#include <iostream>

#include "RelayIMClient.h"
#include "PacketReader.h"
#include "BinaryWriter.h"
#include "Logging.h"

RelayIMClient::RelayIMClient(IRelayIMClientPacketHandler* handler)
    : m_clientNetwork(this), m_handler(handler)
{}

bool RelayIMClient::Initialize()
{
    if (!m_clientNetwork.Initialize())
    {
        return false;
    }

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

void RelayIMClient::OnServerDisconnected()
{
    // TODO(Salads): OnServerDisconnected
}

void RelayIMClient::OnPacketReceived(std::unique_ptr<NetworkPacket> serverPacket)
{
    std::unique_ptr<NetworkPacket> packet = std::move(serverPacket);
    PacketReader reader(packet.get());
    PacketHeader header; reader.ReadHeader(header);

    Log::Get()->ConditionalWriteLine(LOG_NETWORK_PACKET_TYPES, "RECV(%s)", PacketTypeToString(header.m_packetType));

    switch (header.m_packetType)
    {
    case PacketType_ConnectResponse:
    {
        PacketResponseReason responseReason;
        PeerID peerID = INVALID_PEER_ID;
        std::string username;

        reader.ReadPacketResponseReason(responseReason);
        if (responseReason == PacketResponseReason::Success)
        {
            reader.ReadUInt32(peerID);
            reader.ReadString(username);
        }
        
        m_handler->OnRegisterResponse(responseReason, peerID, username);

    } break;
    case PacketType_ListChatRooms_Result:
    {
        std::unique_ptr<std::vector<ChatRoomInfo>> rooms = std::make_unique<std::vector<ChatRoomInfo>>();

        uint16_t nRooms = 0; reader.ReadUInt16(nRooms);
        for (int i = 0; i < nRooms; i++)
        {
            uint32_t roomID = INVALID_ROOM_ID; reader.ReadUInt32(roomID);
            std::string roomName; reader.ReadString(roomName);

            rooms->emplace_back(roomID, roomName);
        }

        m_handler->OnListChatRoomsResponse(std::move(rooms));

    } break;
    case PacketType_JoinChatRoomResponse:
    {
        PacketResponseReason result; reader.ReadPacketResponseReason(result);
        RoomID newRoomID = INVALID_ROOM_ID;
        std::string newRoomname;

        if (result == PacketResponseReason::Success)
        {
            reader.ReadUInt32(newRoomID);
            reader.ReadString(newRoomname);
        }

        m_handler->OnJoinRoomResponse(result, newRoomID, newRoomname);

    } break;
    case PacketType_CreateChatRoomResponse:
    {
        PacketResponseReason result; reader.ReadPacketResponseReason(result);
        RoomID newRoomID = INVALID_ROOM_ID;
        std::string newRoomname;

        if (result == PacketResponseReason::Success)
        {
            reader.ReadUInt32(newRoomID);
            reader.ReadString(newRoomname);
        }

        m_handler->OnCreateRoomResponse(result, newRoomID, newRoomname);

    } break;
    case PacketType_RoomUpdate_MSG:
    {
        RoomID roomID = INVALID_ROOM_ID;   reader.ReadUInt32(roomID);
        PeerID peerID = INVALID_PEER_ID;   reader.ReadUInt32(peerID);
        std::string message; reader.ReadString(message);

        m_handler->OnRoomUpdate_NewMessage(roomID, peerID, message);
        
    } break;
    case PacketType_RoomUpdate_FULL:
    {
        RoomID roomID = INVALID_ROOM_ID;   reader.ReadUInt32(roomID);

        // ARRAY of users in chat room (some users might not have sent a message)
        uint16_t nUsers = 0; reader.ReadUInt16(nUsers);
        for (int i = 0; i < nUsers; i++)
        {
            PeerID userID; reader.ReadUInt32(userID);
            std::string username; reader.ReadString(username);

            m_handler->OnRoomUpdate_UserJoined(roomID, userID, username);
        }

        std::unique_ptr<std::vector<ChatMessage>> messages = std::make_unique<std::vector<ChatMessage>>();
        uint16_t nMessages = 0; reader.ReadUInt16(nMessages);
        for (int i = 0; i < nMessages; i++)
        {
            PeerID userID; reader.ReadUInt32(userID);
            std::string message; reader.ReadString(message);

            messages->emplace_back(userID, message);
        }

        m_handler->OnRoomUpdate_FullUpdate(roomID, std::move(messages));

    } break;
    case PacketType_RoomUpdate_UserJoined:
    {
        RoomID roomID = INVALID_ROOM_ID; reader.ReadUInt32(roomID);
        PeerID peerID = INVALID_PEER_ID; reader.ReadUInt32(peerID);
        std::string username; reader.ReadString(username);

        m_handler->OnRoomUpdate_UserJoined(roomID, peerID, username);

    } break;
    case PacketType_RoomUpdate_UserLeft:
    {
        RoomID roomID = INVALID_ROOM_ID; reader.ReadUInt32(roomID);
        PeerID peerID = INVALID_PEER_ID; reader.ReadUInt32(peerID);

        m_handler->OnRoomUpdate_UserLeft(roomID, peerID);

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
    Log::Get()->ConditionalWriteLine(LOG_UI, "Sending '%s' to Room %u", message, roomID);

    PacketData packetData;
    BinaryWriter writer(packetData);
    writer.WriteHeader(PacketType_SendMessage);
    writer.WriteUInt32(roomID);
    writer.WriteString(message);
    writer.Finalize();

    m_clientNetwork.Send(packetData);
}
