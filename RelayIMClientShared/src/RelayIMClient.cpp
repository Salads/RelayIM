#include <iostream>

#include "RelayIMClient.h"
#include "PacketReader.h"
#include "Logging.h"

bool RelayIMClient::Start()
{
    if (!m_clientNetwork.Start())
    {
        return false;
    }

    m_clientNetwork.OnPacketReceived = [this](std::unique_ptr<NetworkPacket> packet)
    {
        HandleServerPacket(std::move(packet));
    };

    return true;
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
            reader.ReadUInt32(m_peerID);
            reader.ReadString(m_username);
        }
        else if (result == PacketResponseReason::UsernameTaken)
        {
            // TODO(Salads): Connect failure
        }

    } break;
    case PacketType_ListChatRooms_Result:
    {
        m_chatRooms.clear();

        uint16_t nRooms = 0; reader.ReadUInt16(nRooms);
        for (int i = 0; i < nRooms; i++)
        {
            uint32_t roomID = 0; reader.ReadUInt32(roomID);
            std::string roomName; reader.ReadString(roomName);
            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "Room %u: %s\n", roomID, roomName);

            m_chatRooms.emplace_back(roomID, roomName);
        }

        if (!nRooms)
        {
            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "No chat rooms exist yet!\n");
        }

        // TODO(Salads): Event for list chat rooms result?

    } break;
    case PacketType_JoinChatRoomResponse:
    {
        uint8_t result; reader.ReadUInt8(result);
        if (result == PacketResponseReason::Success)
        {
            RoomID newRoomID; reader.ReadUInt32(newRoomID);
            std::string newRoomname; reader.ReadString(newRoomname);

            m_joinedChatRooms[newRoomID] = ClientChatRoom(newRoomID, newRoomname);
            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "Joined room '%s'\n", newRoomname);
        }
        else
        {
            // TODO(Salads): Error
        }
    } break;
    case PacketType_CreateChatRoomResponse:
    {
        uint8_t result; reader.ReadUInt8(result);
        if (result == PacketResponseReason::Success)
        {
            RoomID newRoomID = INVALID_ROOM_ID; reader.ReadUInt32(newRoomID);
            std::string newRoomname; reader.ReadString(newRoomname);
            m_joinedChatRooms[newRoomID] = ClientChatRoom(newRoomID, newRoomname);
            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "Created room '%s'\n", newRoomname);
        }
        else if(result == PacketResponseReason::ChatRoomNameTaken)
        {
            // TODO(Salads): Error
            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "Could not create chat room. Name was taken\n");
        }
        else
        {
            // TODO(Salads): Error
        }
    } break;
    case PacketType_RoomUpdate_MSG:
    {
        RoomID roomID = 0;   reader.ReadUInt32(roomID);
        PeerID peerID = 0;   reader.ReadUInt32(peerID);
        std::string message; reader.ReadString(message);

        if (m_joinedChatRooms.contains(roomID))
        {
            ClientChatRoom* room = &m_joinedChatRooms[roomID];
            room->m_messages.emplace_back(peerID, message);
            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "New Message (Room %u): '%s'\n", roomID, message);
        }
        else
        {
            // TODO(Salads): Room doesnt exist! (joined room not received yet)
        }
        
    } break;
    case PacketType_RoomUpdate_FULL:
    {

        RoomID roomID = 0;   reader.ReadUInt32(roomID);

        ClientChatRoom* room = &m_joinedChatRooms[roomID];

        // ARRAY of users in chat room (some user might not have sent a message)
        uint16_t nUsers = 0; reader.ReadUInt16(nUsers);
        for (int i = 0; i < nUsers; i++)
        {
            PeerID userID; reader.ReadUInt32(userID);
            std::string username; reader.ReadString(username);
            room->m_users.insert(userID);

            UpdateUsersForRoom(userID, roomID, username, false);
        }

        uint16_t nMessages = 0; reader.ReadUInt16(nMessages);
        for (int i = 0; i < nMessages; i++)
        {
            PeerID userID; reader.ReadUInt32(userID);
            std::string message; reader.ReadString(message);
            room->m_messages.emplace_back(userID, message);
            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "Message %d: '%s'\n", i, message);
        }
    } break;
    case PacketType_RoomUpdate_UserJoined:
    {
        RoomID roomID = 0; reader.ReadUInt32(roomID);
        PeerID peerID = 0; reader.ReadUInt32(peerID);
        std::string username; reader.ReadString(username);
        
        UpdateUsersForRoom(peerID, roomID, username, false);

        LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "User '%s' (id %u) joined room %u\n", username, peerID, roomID);
    } break;
    case PacketType_RoomUpdate_UserLeft:
    {
        RoomID roomID = 0; reader.ReadUInt32(roomID);
        PeerID peerID = 0; reader.ReadUInt32(peerID);

        // NOTE(Salads): Don't like the empty string here... if packets arrive out of order, new ClientUser will be created, just to be removed.
        UpdateUsersForRoom(peerID, roomID, "", true);

        LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "User id %u left room %u\n", peerID, roomID);
    } break;
    default:
    {

    } break;
    }
}

void RelayIMClient::UpdateUsersForRoom(PeerID peerID, RoomID roomID, std::string username, bool remove = false)
{
    ClientUser* user;
    if (m_users.contains(peerID))
    {
        user = &m_users[peerID];
    }
    else
    {
        m_users[peerID] = ClientUser(peerID, username);
        user = &m_users[peerID];
    }

    if (remove)
    {
        user->RemoveFromRoom(roomID);
    }
    else
    {
        user->AddToRoom(roomID);
    }

    if (user->GetNumRooms() == 0)
    {
        m_users.erase(peerID);
    }
}