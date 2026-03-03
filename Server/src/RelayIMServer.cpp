#include "RelayIMServer.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#include "Util.h"
#include "PacketType.h"
#include "NetworkTypes.h"
#include "PacketReader.h"
#include "BinaryWriter.h"
#include "Logging.h"

bool RelayIMServer::Initialize()
{
    if (m_isInitialized) { return true; }

    if (!m_serverNetwork.Initialize())
    {
        std::cerr << "Failed to initialize server network interface..." << std::endl;
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

    m_serverNetwork.OnPacketReceived = [this](PeerID peerID, std::unique_ptr<NetworkPacket> packet)
    {
        this->AddPacketToQueue(std::move(packet));
    };

    std::cout << "Server Initialized" << std::endl;
    m_isInitialized = true;
    return true;
}

bool RelayIMServer::Start()
{
    if (!GetIsInitialized())
    {
        Initialize();

        if (!GetIsInitialized())
        {
            return false;
        }
    }

    std::cout << "Starting Server..." << std::endl;
    std::cout << "\tStarting Packet Handler Thread" << std::endl;
    
    m_packetHandlerThread = std::thread(&RelayIMServer::ProcessClientPackets, this);
    m_running = true;

    std::cout << "Server started!" << std::endl;
    return true;
}

void RelayIMServer::Stop()
{
    m_running = false;
    m_incomingPacketsCV.notify_one();

    std::cout << "Shutting down server network interface..." << std::endl;
    m_serverNetwork.Shutdown();

    std::cout << "Cleaning up already disconnected clients" << std::endl;
    m_serverNetwork.DeleteDisconnectedClients();

    std::cout << "Joining packet handler thread..." << std::endl;
    if (m_packetHandlerThread.joinable())
    {
        m_packetHandlerThread.join();
    }

    std::cout << "Stopped RelayIMServer" << std::endl;
}

void RelayIMServer::Update()
{
    m_serverNetwork.DeleteDisconnectedClients();
}

void RelayIMServer::ProcessClientPackets()
{
    while (m_running)
    {
        std::unique_lock cvLock(m_incomingPacketsMutex);
        m_incomingPacketsCV.wait(cvLock, [this]() {
            return !m_running.load() || !m_incomingPackets.empty();
        });

        if (!m_running)
        {
            cvLock.unlock();
            break;
        }

        std::unique_ptr<NetworkPacket> packet = std::move(m_incomingPackets.front());
        m_incomingPackets.pop();
        cvLock.unlock();

        PacketReader reader(packet.get());

        PeerID peerID = packet.get()->m_peerID;
        PacketHeader header;
        if (!reader.ReadHeader(header))
        {
            continue;
        }

        LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Packet Received (%s): [%X, %u]\n", PacketTypeToString(header.m_packetType), header.m_passCode, header.m_version);

        // Read Packet Payload
        switch (header.m_packetType)
        {
        case PacketType_Connect:
        {
            std::string newUsername;
            if (!reader.ReadString(newUsername))
            {
                break;
            }

            if (!IsUsernameTaken(newUsername))
            {
                {
                    std::lock_guard<std::mutex> lock(m_clientsMutex);

                    std::unique_ptr<ChatClient> newChatClient = std::make_unique<ChatClient>(peerID);
                    newChatClient.get()->m_username = newUsername;
                    newChatClient.get()->m_status = ChatClientStatus_Connected;
                    m_clients[peerID] = std::move(newChatClient);
                }

                PacketData response;
                BinaryWriter writer(response);
                writer.WriteHeader(PacketType_ConnectResponse);
                writer.WriteUInt8(PacketResponseReason::Success);
                writer.WriteUInt32(peerID);
                writer.WriteString(newUsername);
                writer.Finalize();
                m_serverNetwork.SendToClient(peerID, &response);

                LogDepthConditional(LOG_NETWORK_PACKETS, 1, "New client %u registered as '%s'\n", peerID, newUsername);
            }
            else
            {
                PacketData response;
                BinaryWriter writer(response);
                writer.WriteHeader(PacketType_ConnectResponse);
                writer.WriteUInt8(PacketResponseReason::UsernameTaken);
                writer.WriteUInt32(INVALID_PEER_ID);
                writer.WriteString("");
                writer.Finalize();
                m_serverNetwork.SendToClient(peerID, &response);
            }

            break;
        }
        case PacketType_ListChatRooms:
        {
            // TODO(Salads): Server Packet

            // Send PacketType_ListChatRooms_Result -> ARRAY[RoomID, RoomName]
            PacketData response;
            BinaryWriter writer(response);
            writer.WriteHeader(PacketType_ListChatRooms_Result);

            {
                std::lock_guard lock(m_chatRoomsMutex);
                writer.WriteUInt16(static_cast<uint16_t>(m_chatRooms.size()));

                for (const auto& [roomID, chatRoom] : m_chatRooms)
                {
                    writer.WriteUInt32(roomID);

                    std::string roomName = chatRoom->GetRoomName();
                    writer.WriteString(roomName);
                }
            }

            writer.Finalize();

            m_serverNetwork.SendToClient(peerID, &response);

            break;
        } 
        case PacketType_JoinChatRoom:
        {
            uint32_t roomID = 0;
            if (!reader.ReadUInt32(roomID))
            {
                break;
            }

            bool roomExists = false;
            {
                std::lock_guard<std::mutex> lock(m_chatRoomsMutex);
                roomExists = m_chatRooms.contains(roomID);
            }

            if (roomExists)
            {
                std::string joiningUsername;
                {
                    std::lock_guard<std::mutex> lock(m_clientsMutex);
                    joiningUsername = m_clients[peerID]->m_username;
                }

                {
                    std::lock_guard lock(m_chatRoomsMutex);
                    m_chatRooms[roomID]->AddClient(peerID);
                    
                    // Notify all clients of this chat room of the new client
                    std::vector<uint8_t> responsePacket;
                    BinaryWriter writer(responsePacket);
                    writer.WriteHeader(PacketType_RoomUpdate_UserJoined);
                    writer.WriteUInt32(roomID);
                    writer.WriteUInt32(peerID);
                    writer.WriteString(joiningUsername);
                    writer.Finalize();

                    std::vector<PeerID> roomClients = m_chatRooms[roomID]->GetClients();
                    for (PeerID roomClient : roomClients)
                    {
                        m_serverNetwork.SendToClient(roomClient, &responsePacket);
                    }

                    // Send FULL update to newly joined client (arr users, arr messages)
                    std::vector<uint8_t> fullResponse;
                    BinaryWriter fullWriter(fullResponse);
                    fullWriter.WriteHeader(PacketType_RoomUpdate_FULL);
                    fullWriter.WriteUInt32(roomID);
                    fullWriter.WriteUInt16(static_cast<uint16_t>(roomClients.size()));

                    {
                        std::lock_guard lock(m_clientsMutex);
                        for (PeerID roomClient : roomClients)
                        {
                            fullWriter.WriteUInt32(roomClient);
                            fullWriter.WriteString(m_clients[peerID].get()->m_username);
                        }
                    }

                    ChatRoom* chatRoom = m_chatRooms[roomID].get();
                    const std::vector<ChatMessage>* messages = chatRoom->GetMessages();

                    fullWriter.WriteUInt16(messages->size());

                    for (int i = 0; i < messages->size(); i++)
                    {
                        const ChatMessage& message = messages->at(i);
                        fullWriter.WriteUInt32(message.m_senderID);
                        fullWriter.WriteString(message.m_message);
                    }

                    fullWriter.Finalize();
                    m_serverNetwork.SendToClient(peerID, &fullResponse);
                }

                LogDepthConditional(LOG_NETWORK_PACKETS, 1, "Client %u joined room %u\n", peerID, roomID);
            }
            else
            {
                PacketData response;
                BinaryWriter writer(response);
                writer.WriteHeader(PacketType_JoinChatRoomResponse);
                writer.WriteUInt8(PacketResponseReason::ChatRoomDoesntExist);
                writer.Finalize();
                m_serverNetwork.SendToClient(peerID, &response);
            }

            break;
        }
        case PacketType_CreateChatRoom:
        {
            std::string roomName;
            if (!reader.ReadString(roomName))
            {
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

                PacketData response;
                BinaryWriter writer(response);
                writer.WriteHeader(PacketType_CreateChatRoomResponse);
                writer.WriteUInt8(PacketResponseReason::Success);
                writer.WriteUInt32(newRoomID);
                writer.WriteString(roomName);
                writer.Finalize();
                m_serverNetwork.SendToClient(peerID, &response);

                LogDepthConditional(LOG_NETWORK_PACKETS, 1, "Client created roomname: '%s'\n", roomName);
            }
            else
            {
                PacketData response;
                BinaryWriter writer(response);
                writer.WriteHeader(PacketType_CreateChatRoomResponse);
                writer.WriteUInt8(PacketResponseReason::ChatRoomNameTaken);
                writer.WriteUInt32(INVALID_ROOM_ID);
                writer.WriteString("");
                writer.Finalize();
                m_serverNetwork.SendToClient(peerID, &response);
            }

            break;
        }
        case PacketType_LeaveChatRoom:
        {
            uint32_t roomID = 0;
            if (!reader.ReadUInt32(roomID))
            {
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

                LogDepthConditional(LOG_NETWORK_PACKETS, 1, "Client %u left room %u\n", peerID, roomID);

                // Notify chat room members of leaving user
                std::vector<uint8_t> responsePacket;
                BinaryWriter writer(responsePacket);
                writer.WriteHeader(PacketType_RoomUpdate_UserLeft);
                writer.WriteUInt32(roomID);
                writer.WriteUInt32(peerID);
                writer.Finalize();

                {
                    std::lock_guard<std::mutex> lock(m_chatRoomsMutex);
                    std::vector<PeerID> roomClients = m_chatRooms[roomID]->GetClients();

                    for (PeerID roomClient : roomClients)
                    {
                        m_serverNetwork.SendToClient(roomClient, &responsePacket);
                    }
                }
            }

            break;
        }
        case PacketType_SendMessage:
        {
            uint32_t roomID = 0;
            if (!reader.ReadUInt32(roomID))
            {
                break;
            }

            std::string message;
            if (!reader.ReadString(message))
            {
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
                

                LogDepthConditional(LOG_NETWORK_PACKETS, 1, "Client %u sent message '%s' to room %u\n", peerID, message, roomID);

                // Add message to chat room, get all clients in chat room
                std::vector<PeerID> chatRoomClients;
                {
                    std::lock_guard<std::mutex> lock(m_chatRoomsMutex);
                    m_chatRooms[roomID]->AddMessage(newMessage);
                    chatRoomClients = m_chatRooms[roomID]->GetClients();
                }

                // Notify all clients in chat room about the message
                std::vector<uint8_t> messagePacket;
                BinaryWriter writer(messagePacket);
                writer.WriteHeader(PacketType_RoomUpdate_MSG);
                writer.WriteUInt32(roomID);
                writer.WriteUInt32(peerID);
                writer.WriteString(message);
                writer.Finalize();

                for (PeerID chatRoomClientID : chatRoomClients)
                {
                    m_serverNetwork.SendToClient(chatRoomClientID, &messagePacket);
                }
            }

            break;
        }
        default:
        {
            LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Client %u sent unknown packet type: %s\n", peerID, PacketTypeToString(header.m_packetType));
            break;
        }
        }
    }
}

void RelayIMServer::AddPacketToQueue(std::unique_ptr<NetworkPacket> newPacket)
{
    std::lock_guard lock(m_incomingPacketsMutex);
    m_incomingPackets.push(std::move(newPacket));
    m_incomingPacketsCV.notify_one();
}

bool RelayIMServer::IsRoomnameTaken(std::string& newRoomname)
{
    std::lock_guard<std::mutex> lock(m_chatRoomsMutex);
    for (auto &[roomID, chatRoom] : m_chatRooms)
    {
        if (chatRoom->GetRoomName() == newRoomname)
        {
            return true;
        }
    }

    return false;
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

bool RelayIMServer::GetIsInitialized() const
{
    return m_isInitialized;
}

