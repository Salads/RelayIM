#include "RelayIMServer.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#include "Util.h"
#include "PacketType.h"
#include "NetworkTypes.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"

bool RelayIMServer::Initialize()
{
    if (m_isInitialized) { return true; }

    if (!m_serverNetwork.Initialize())
    {
        std::cerr << "Failed to initialize server peer" << std::endl;
        return false;
    }

    m_serverNetwork.OnNewClient = [this](PeerID newPeerID) 
    {
        HandleNewClient(newPeerID);
    };

    m_serverNetwork.OnClientDisconnected = [this](PeerID peerID)
    {
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        m_clients.erase(peerID);
    };

    m_serverNetwork.OnPacketReceived = [this](PeerID peerID, std::vector<uint8_t>* packet)
    {
        HandleClientPacket(peerID, packet);
    };

    std::cout << "Server Initialized" << std::endl;
    m_isInitialized = true;
    return true;
}

void RelayIMServer::SendSimpleResponsePacket(PeerID peerID, bool success)
{
    std::vector<uint8_t> responsePacket;
    BinaryWriter writer(responsePacket);
    writer.WriteUInt32(NETWORK_PASSCODE);
    writer.WriteUInt8(NETWORK_VERSION);
    writer.WriteUInt8(PacketType_Response);
    writer.WriteUInt8(success);
    writer.Finalize();

    m_serverNetwork.SendToClient(peerID, &responsePacket);
}

void RelayIMServer::HandleClientPacket(PeerID peerID, std::vector<uint8_t>* packet)
{
    // TODO(Salads): Process packet and respond accordingly

    BinaryReader reader(packet);

    // Read Packet Header
    uint16_t packetSize = 0; reader.ReadUInt16(packetSize);
    uint32_t passCode = 0; reader.ReadUInt32(passCode);
    uint8_t version = 0; reader.ReadUInt8(version);
    uint8_t packetType = 0; reader.ReadUInt8(packetType);

    // Read Packet Payload
    switch (packetType)
    {
        case PacketType_Connect:
        {
            std::string newUsername;
            if (!reader.ReadString(newUsername)) 
            {
                SendSimpleResponsePacket(peerID, false);
                break;
            }

            if (!IsUsernameTaken(newUsername))
            {
                {
                    std::lock_guard<std::mutex> lock(m_clientsMutex);

                    std::unique_ptr<ChatClient> newChatClient;
                    newChatClient.get()->m_username = newUsername;
                    newChatClient.get()->m_status = ChatClientStatus_Connected;
                    m_clients[peerID] = std::move(newChatClient);
                }

                SendSimpleResponsePacket(peerID, true);
            }
            else
            {
                SendSimpleResponsePacket(peerID, false);
            }

            break;
        }
        case PacketType_JoinChatRoom:
        {
            uint32_t roomID = 0;
            if (!reader.ReadUInt32(roomID))
            {
                SendSimpleResponsePacket(peerID, false);
                break;
            }

            bool roomExists = false;
            {
                std::lock_guard<std::mutex> lock(m_chatRoomsMutex);
                roomExists = m_chatRooms.contains(roomID);
            }

            if (roomExists)
            {
                m_chatRooms[roomID]->AddClient(peerID);
                SendSimpleResponsePacket(peerID, true);

                std::string joiningUsername;
                {
                    std::lock_guard<std::mutex> lock(m_clientsMutex);
                    joiningUsername = m_clients[peerID]->m_username;
                }

                // Notify all clients of this chat room of the new client
                std::vector<uint8_t> responsePacket;
                BinaryWriter writer(responsePacket);
                writer.WriteUInt32(NETWORK_PASSCODE);
                writer.WriteUInt8(NETWORK_VERSION);
                writer.WriteUInt8(PacketType_RoomUpdate_UserJoined);
                writer.WriteUInt32(roomID);
                writer.WriteUInt32(peerID);
                writer.WriteString(joiningUsername);
                writer.Finalize();

                std::vector<PeerID> roomClients;
                {
                    std::lock_guard<std::mutex> lock(m_chatRoomsMutex);
                    roomClients = m_chatRooms[roomID]->GetClients();
                }

                for (PeerID roomClient : roomClients)
                {
                    if (roomClient != peerID)
                    {
                        m_serverNetwork.SendToClient(roomClient, &responsePacket);
                    }
                }

                // TODO(Salads): Give the new peer a FULL room update. (users, messages) 
                // PacketType_RoomUpdate_MSG_FULL
            }
            else
            {
                SendSimpleResponsePacket(peerID, false);
            }

            break;
        }
        case PacketType_CreateChatRoom:
        {
            std::string roomName;
            if (!reader.ReadString(roomName))
            {
                SendSimpleResponsePacket(peerID, false);
                break;
            }

            if (!IsRoomnameTaken(roomName))
            {
                RoomID newRoomID = m_nextRoomID++;
                std::unique_ptr<ChatRoom> newChatRoom = std::make_unique<ChatRoom>(newRoomID, roomName);
                {
                    std::lock_guard<std::mutex> lock(m_chatRoomsMutex);
                    m_chatRooms[newRoomID] = std::move(newChatRoom);
                    m_chatRooms[newRoomID]->AddClient(peerID);
                }

                SendSimpleResponsePacket(peerID, true);
            }
            else
            {
                SendSimpleResponsePacket(peerID, false);
            }

            break;
        }
        case PacketType_LeaveChatRoom:
        {
            uint32_t roomID = 0;
            if (!reader.ReadUInt32(roomID))
            {
                SendSimpleResponsePacket(peerID, false);
                break;
            }

            bool roomExists = false;
            {
                std::lock_guard<std::mutex> lock(m_chatRoomsMutex);
                roomExists = m_chatRooms.contains(roomID);
            }

            if (roomExists)
            {
                m_chatRooms[roomID]->RemoveClient(peerID);
                SendSimpleResponsePacket(peerID, true);

                // Notify chat room members of leaving user
                std::vector<uint8_t> responsePacket;
                BinaryWriter writer(responsePacket);
                writer.WriteUInt32(NETWORK_PASSCODE);
                writer.WriteUInt8(NETWORK_VERSION);
                writer.WriteUInt8(PacketType_RoomUpdate_UserLeft);
                writer.WriteUInt32(roomID);
                writer.WriteUInt32(peerID);
                writer.Finalize();

                std::vector<PeerID> roomClients;
                {
                    std::lock_guard<std::mutex> lock(m_chatRoomsMutex);
                    roomClients = m_chatRooms[roomID]->GetClients();
                }

                for (PeerID roomClient : roomClients)
                {
                    m_serverNetwork.SendToClient(roomClient, &responsePacket);
                }
            }
            else
            {
                SendSimpleResponsePacket(peerID, false);
            }

            break;
        }
        case PacketType_SendMessage:
        {
            uint32_t roomID = 0;
            if (!reader.ReadUInt32(roomID))
            {
                SendSimpleResponsePacket(peerID, false);
                break;
            }

            std::string message;
            if (!reader.ReadString(message))
            {
                SendSimpleResponsePacket(peerID, false);
                break;
            }

            bool roomExists = false;
            {
                std::lock_guard<std::mutex> lock(m_chatRoomsMutex);
                roomExists = m_chatRooms.contains(roomID);
            }

            if (roomExists)
            {
                ChatMessage newMessage(peerID, message);
                std::vector<PeerID> chatRoomClients;

                // Add message to chat room, get all clients in chat room
                {
                    std::lock_guard<std::mutex> lock(m_chatRoomsMutex);
                    m_chatRooms[roomID]->AddMessage(newMessage);
                    chatRoomClients = m_chatRooms[roomID]->GetClients();
                }

                // Notify all clients in chat room about the message
                std::vector<uint8_t> messagePacket;
                BinaryWriter writer(messagePacket);
                writer.WriteUInt32(NETWORK_PASSCODE);
                writer.WriteUInt8(NETWORK_VERSION);
                writer.WriteUInt8(PacketType_RoomUpdate_MSG);
                writer.WriteUInt32(roomID);
                writer.WriteString(message);
                writer.WriteUInt32(peerID);
                writer.Finalize();
                
                for (PeerID chatRoomClientID : chatRoomClients)
                {
                    m_serverNetwork.SendToClient(chatRoomClientID, &messagePacket);
                }
            }
            else
            {
                SendSimpleResponsePacket(peerID, false);
            }

            break;
        }
        default:
        {
            std::cout << "Client " << peerID << " sent unknown packet type: " << (int)packetType << std::endl;
            break;
        }
    }
}

bool RelayIMServer::IsRoomnameTaken(std::string& newRoomname)
{
    std::lock_guard<std::mutex> lock(m_chatRoomsMutex);
    for (auto &[roomID, chatRoom] : m_chatRooms)
    {
        if (chatRoom->GetRoomName() == newRoomname)
        {
            return false;
        }
    }

    return true;
}

bool RelayIMServer::IsUsernameTaken(std::string &newUsername)
{
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    for (auto& [peerID, clientPtr] : m_clients)
    {
        if (clientPtr->m_username == newUsername)
        {
            return true;
        }
    }

    return false;
}

void RelayIMServer::HandleNewClient(PeerID newPeerID)
{
    std::unique_ptr<ChatClient> newChatClient = std::make_unique<ChatClient>(newPeerID);
    {
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        m_clients.emplace(newPeerID, std::move(newChatClient));
    }
}

void RelayIMServer::Stop()
{
    m_serverNetwork.Shutdown();
}

bool RelayIMServer::IsInitialized() const
{
    return m_isInitialized;
}

bool RelayIMServer::Start()
{
    if (!IsInitialized()) 
    { 
        Initialize();

        if (!IsInitialized())
        {
            std::cerr << "Failed to initialize server" << std::endl;
            return false;
        }
    }
    
    // TODO(Salads): Receive data from clients and handle it.

    return true;
}
