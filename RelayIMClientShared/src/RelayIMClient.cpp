#include <iostream>

#include "RelayIMClient.h"
#include "PacketReader.h"
#include "BinaryWriter.h"
#include "Logging.h"

bool RelayIMClient::Initialize()
{
    if (!m_clientNetwork.Initialize())
    {
        return false;
    }

    m_clientNetwork.OnPacketReceived = [this](std::unique_ptr<NetworkPacket> packet)
    {
        HandleServerPacket(std::move(packet));
    };

    return true;
}

bool RelayIMClient::Connect()
{
    return m_clientNetwork.Connect();
}

void RelayIMClient::Shutdown()
{
    m_clientNetwork.Shutdown();
}

void RelayIMClient::HandleServerPacket(std::unique_ptr<NetworkPacket> serverPacket)
{
    std::unique_ptr<NetworkPacket> packet = std::move(serverPacket);
    PacketReader reader(packet.get());
    PacketHeader header; reader.ReadHeader(header);

    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Packet Received (%s): [%X, %u]\n", PacketTypeToString(header.m_packetType), header.m_passCode, header.m_version);

    switch (header.m_packetType)
    {
    case PacketType_ConnectResponse:
    {
        uint8_t result; reader.ReadUInt8(result);
        if (result == PacketResponseReason::Success)
        {
            PeerID peerID; reader.ReadUInt32(peerID);
            std::string username; reader.ReadString(username);

            if(OnLocalRegistered)
            {
                OnLocalRegistered(peerID, username);
            }
        }
        else
        {
            if (OnResponseFailed)
            {
                OnResponseFailed(static_cast<PacketResponseReason>(result));
            }
        }

    } break;
    case PacketType_ListChatRooms_Result:
    {
        std::unique_ptr<std::vector<ChatRoomInfo>> rooms = std::make_unique<std::vector<ChatRoomInfo>>();

        uint16_t nRooms = 0; reader.ReadUInt16(nRooms);
        for (int i = 0; i < nRooms; i++)
        {
            uint32_t roomID = 0; reader.ReadUInt32(roomID);
            std::string roomName; reader.ReadString(roomName);

            rooms->emplace_back(roomID, roomName);

            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "Room %u: %s\n", roomID, roomName);
        }

        if (OnListChatRoomsReceived)
        {
            OnListChatRoomsReceived(std::move(rooms));
        }

        if (!nRooms)
        {
            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "No chat rooms exist yet!\n");
        }

    } break;
    case PacketType_JoinChatRoomResponse:
    {
        uint8_t result; reader.ReadUInt8(result);
        if (result == PacketResponseReason::Success)
        {
            RoomID newRoomID; reader.ReadUInt32(newRoomID);
            std::string newRoomname; reader.ReadString(newRoomname);

            if (OnLocalChatRoomJoined)
            {
                OnLocalChatRoomJoined(newRoomID, newRoomname);
            }

            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "Joined room '%s'\n", newRoomname);
        }
        else
        {
            if (OnResponseFailed)
            {
                OnResponseFailed(static_cast<PacketResponseReason>(result));
            }
        }
    } break;
    case PacketType_CreateChatRoomResponse:
    {
        uint8_t result; reader.ReadUInt8(result);
        if (result == PacketResponseReason::Success)
        {
            RoomID newRoomID = INVALID_ROOM_ID; reader.ReadUInt32(newRoomID);
            std::string newRoomname; reader.ReadString(newRoomname);

            if (OnLocalChatRoomJoined)
            {
                OnLocalChatRoomJoined(newRoomID, newRoomname);
            }

            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "Created room '%s'\n", newRoomname);
        }
        else if(result == PacketResponseReason::ChatRoomNameTaken)
        {
            
            if (OnResponseFailed)
            {
                OnResponseFailed(static_cast<PacketResponseReason>(result));
            }

            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "Could not create chat room. Name was taken\n");
        }
        else
        {
            if (OnResponseFailed)
            {
                OnResponseFailed(PacketResponseReason::Unspecified);
            }
        }
    } break;
    case PacketType_RoomUpdate_MSG:
    {
        RoomID roomID = 0;   reader.ReadUInt32(roomID);
        PeerID peerID = 0;   reader.ReadUInt32(peerID);
        std::string message; reader.ReadString(message);

        if (OnMessageReceived)
        {
            OnMessageReceived(peerID, roomID, message);
        }

        LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "New Message (Room %u): '%s'\n", roomID, message);
        
    } break;
    case PacketType_RoomUpdate_FULL:
    {

        RoomID roomID = 0;   reader.ReadUInt32(roomID);

        // ARRAY of users in chat room (some user might not have sent a message)
        uint16_t nUsers = 0; reader.ReadUInt16(nUsers);
        for (int i = 0; i < nUsers; i++)
        {
            PeerID userID; reader.ReadUInt32(userID);
            std::string username; reader.ReadString(username);

            if (OnNewClientDiscovered)
            {
                OnNewClientDiscovered(userID, roomID, username);
            }
        }

        std::unique_ptr<std::vector<ChatMessage>> messages = std::make_unique<std::vector<ChatMessage>>();
        uint16_t nMessages = 0; reader.ReadUInt16(nMessages);
        for (int i = 0; i < nMessages; i++)
        {
            PeerID userID; reader.ReadUInt32(userID);
            std::string message; reader.ReadString(message);

            messages->emplace_back(userID, message);

            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "Message %d: '%s'\n", i, message);
        }

        if (OnRoomFullUpdate)
        {
            OnRoomFullUpdate(roomID, std::move(messages));
        }

    } break;
    case PacketType_RoomUpdate_UserJoined:
    {
        RoomID roomID = 0; reader.ReadUInt32(roomID);
        PeerID peerID = 0; reader.ReadUInt32(peerID);
        std::string username; reader.ReadString(username);

        if (OnNewClientDiscovered)
        {
            OnNewClientDiscovered(peerID, roomID, username);
        }

        LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "User '%s' (id %u) joined room %u\n", username, peerID, roomID);
    } break;
    case PacketType_RoomUpdate_UserLeft:
    {
        RoomID roomID = 0; reader.ReadUInt32(roomID);
        PeerID peerID = 0; reader.ReadUInt32(peerID);

        if (OnClientLeftChatRoom)
        {
            OnClientLeftChatRoom(peerID, roomID);
        }

        LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "User id %u left room %u\n", peerID, roomID);
    } break;
    default:
    {

    } break;
    }
}

void RelayIMClient::SendConnect(std::string desiredUsername)
{
    PacketData packetData;
    BinaryWriter writer(packetData);
    writer.WriteHeader(PacketType_Connect);
    writer.WriteString(desiredUsername);
    writer.Finalize();

    m_clientNetwork.Send(packetData);
}

void RelayIMClient::SendRequestAllChatRooms()
{
    PacketData packetData;
    BinaryWriter writer(packetData);
    writer.WriteHeader(PacketType_ListChatRooms);
    writer.Finalize();

    m_clientNetwork.Send(packetData);
}

void RelayIMClient::SendJoinChatRoom(RoomID roomID)
{
    PacketData packetData;
    BinaryWriter writer(packetData);
    writer.WriteHeader(PacketType_JoinChatRoom);
    writer.WriteUInt32(roomID);
    writer.Finalize();

    m_clientNetwork.Send(packetData);
}

void RelayIMClient::SendCreateChatRoom(std::string roomName)
{
    PacketData packetData;
    BinaryWriter writer(packetData);
    writer.WriteHeader(PacketType_CreateChatRoom);
    writer.WriteString(roomName);
    writer.Finalize();

    m_clientNetwork.Send(packetData);
}

void RelayIMClient::SendLeaveChatRoom(RoomID roomID)
{
    PacketData packetData;
    BinaryWriter writer(packetData);
    writer.WriteHeader(PacketType_LeaveChatRoom);
    writer.WriteUInt32(roomID);
    writer.Finalize();

    m_clientNetwork.Send(packetData);
}

void RelayIMClient::SendMessageToRoom(RoomID roomID, std::string message)
{
    PacketData packetData;
    BinaryWriter writer(packetData);
    writer.WriteHeader(PacketType_SendMessage);
    writer.WriteUInt32(roomID);
    writer.WriteString(message);
    writer.Finalize();

    m_clientNetwork.Send(packetData);
}
