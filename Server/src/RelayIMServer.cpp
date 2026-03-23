#include "RelayIMServer.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#include "Util.h"
#include "PacketType.h"
#include "NetworkTypes.h"
#include "PacketReader.h"
#include "PacketWriter.h"
#include "Logging.h"

RelayIMServer::RelayIMServer()
    : m_serverNetwork(this)
{}

bool RelayIMServer::Initialize()
{
    if (GetIsInitialized()) { return true; }

    if (!m_serverNetwork.Initialize())
    {
        std::cerr << "Failed to initialize server network interface..." << std::endl;
        return false;
    }

    std::cout << "Server Initialized" << std::endl;
    m_isInitialized = true;
    return true;
}

void RelayIMServer::OnNewClient(PeerID newPeerID)
{
    HandleNewClient(newPeerID);
}

void RelayIMServer::OnClientDisconnected(PeerID peerID)
{
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    m_clients.erase(peerID);
}

void RelayIMServer::OnPacketReceived(PeerID peerID, std::unique_ptr<NetworkPacket> packet)
{
    this->AddPacketToQueue(std::move(packet));
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

    if (m_running)
    {
        return false;
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

        Log::Get()->ConditionalWriteLine(LOG_NETWORK_PACKET_TYPES, "RECV(%s) from Peer(%u)", PacketTypeToString(header.m_packetType), peerID);

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
                PacketWriter writer(response);
                writer.WriteHeader(PacketType_ConnectResponse);
                writer.WriteUInt8(PacketResponseReason::Success);
                writer.WriteUInt32(peerID);
                writer.WriteString(newUsername);
                writer.Finalize();
                m_serverNetwork.SendToClient(peerID, &response);
            }
            else
            {
                PacketData response;
                PacketWriter writer(response);
                writer.WriteHeader(PacketType_ConnectResponse);
                writer.WriteUInt8(PacketResponseReason::UsernameTaken);
                writer.Finalize();
                m_serverNetwork.SendToClient(peerID, &response);
            }

            break;
        }
        case PacketType_ListChatRooms:
        {
            // Send PacketType_ListChatRooms_Result -> ARRAY[RoomID, RoomName]
            PacketData response;
            PacketWriter writer(response);
            writer.WriteHeader(PacketType_ListChatRooms_Result);
            writer.WriteUInt16(static_cast<uint16_t>(m_chatRooms.size()));

            for(const auto& [roomID, chatRoom] : m_chatRooms)
            {
                writer.WriteUInt32(roomID);

                std::string roomName = chatRoom->GetRoomName();
                writer.WriteString(roomName);
            }

            writer.Finalize();

            m_serverNetwork.SendToClient(peerID, &response);
        } break;
        case PacketType_JoinChatRoom:
        {
            uint32_t roomID = 0;
            if (!reader.ReadUInt32(roomID))
            {
                break;
            }

            if (m_chatRooms.contains(roomID))
            {
                std::string joiningUsername;
                {
                    std::lock_guard<std::mutex> lock(m_clientsMutex);
                    joiningUsername = m_clients[peerID]->m_username;
                }

                // Send JoinRoomResponse to joiner
                std::vector<uint8_t> joinerResponse;
                PacketWriter joinerResponseWriter(joinerResponse);
                joinerResponseWriter.WriteHeader(PacketType_JoinChatRoomResponse);
                joinerResponseWriter.WriteUInt8(PacketResponseReason::Success);
                joinerResponseWriter.WriteUInt32(roomID);
                joinerResponseWriter.WriteString(m_chatRooms[roomID]->GetRoomName());
                joinerResponseWriter.Finalize();
                m_serverNetwork.SendToClient(peerID, &joinerResponse);

                // Notify all clients (except joiner) of this chat room of the new client
                std::vector<uint8_t> newUserResponse;
                PacketWriter newUserWriter(newUserResponse);
                newUserWriter.WriteHeader(PacketType_RoomUpdate_UserJoined);
                newUserWriter.WriteUInt32(roomID);
                newUserWriter.WriteUInt32(peerID);
                newUserWriter.WriteString(joiningUsername);
                newUserWriter.Finalize();

                std::vector<PeerID> roomClients = m_chatRooms[roomID]->GetClients();
                for(PeerID roomClient : roomClients)
                {
                    m_serverNetwork.SendToClient(roomClient, &newUserResponse);
                }

                // Send FULL update to newly joined client (arr users, arr messages)
                std::vector<uint8_t> fullResponse;
                PacketWriter fullWriter(fullResponse);
                fullWriter.WriteHeader(PacketType_RoomUpdate_FULL);
                fullWriter.WriteUInt32(roomID);
                fullWriter.WriteUInt16(static_cast<uint16_t>(roomClients.size()));

                {
                    std::lock_guard lock(m_clientsMutex);
                    for(PeerID roomClient : roomClients)
                    {
                        fullWriter.WriteUInt32(roomClient);
                        fullWriter.WriteString(m_clients[peerID].get()->m_username);
                    }
                }

                ChatRoom* chatRoom = m_chatRooms[roomID].get();
                const std::vector<ChatMessage>* messages = chatRoom->GetMessages();

                uint16_t nMessages = static_cast<uint16_t>(messages->size());
                fullWriter.WriteUInt16(nMessages);

                for(int i = 0; i < nMessages; i++)
                {
                    const ChatMessage& message = messages->at(i);
                    fullWriter.WriteUInt32(message.m_senderID);
                    fullWriter.WriteString(message.m_message);
                }

                fullWriter.Finalize();
                m_serverNetwork.SendToClient(peerID, &fullResponse);
                m_chatRooms[roomID]->AddClient(peerID);

            }
            else
            {
                PacketData response;
                PacketWriter writer(response);
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
                m_chatRooms[newRoomID] = std::move(newChatRoom);
                m_chatRooms[newRoomID]->AddClient(peerID);

                PacketData response;
                PacketWriter writer(response);
                writer.WriteHeader(PacketType_CreateChatRoomResponse);
                writer.WriteUInt8(PacketResponseReason::Success);
                writer.WriteUInt32(newRoomID);
                writer.WriteString(roomName);
                writer.Finalize();
                m_serverNetwork.SendToClient(peerID, &response);

            }
            else
            {
                PacketData response;
                PacketWriter writer(response);
                writer.WriteHeader(PacketType_CreateChatRoomResponse);
                writer.WriteUInt8(PacketResponseReason::ChatRoomNameTaken);
                writer.Finalize();
                m_serverNetwork.SendToClient(peerID, &response);
            }

            break;
        }
        case PacketType_LeaveChatRoom:
        {
            uint32_t roomID = INVALID_ROOM_ID;
            if (!reader.ReadUInt32(roomID))
            {
                break;
            }

            if (m_chatRooms.contains(roomID))
            {
                // Notify chat room members of leaving user
                std::vector<uint8_t> responsePacket;
                PacketWriter writer(responsePacket);
                writer.WriteHeader(PacketType_RoomUpdate_UserLeft);
                writer.WriteUInt32(roomID);
                writer.WriteUInt32(peerID);
                writer.Finalize();

                std::vector<PeerID> roomClients = m_chatRooms[roomID]->GetClients();
                for(PeerID roomClient : roomClients)
                {
                    m_serverNetwork.SendToClient(roomClient, &responsePacket);
                }

                m_chatRooms[roomID]->RemoveClient(peerID);
            }

            break;
        }
        case PacketType_SendMessage:
        {
            uint32_t roomID = INVALID_ROOM_ID;
            if (!reader.ReadUInt32(roomID))
            {
                break;
            }

            std::string message;
            if (!reader.ReadString(message))
            {
                break;
            }

            if (m_chatRooms.contains(roomID))
            {
                ChatMessage newMessage(peerID, message);

                // Add message to chat room, get all clients in chat room
                std::vector<PeerID> chatRoomClients = m_chatRooms[roomID]->GetClients();
                m_chatRooms[roomID]->AddMessage(newMessage);

                // Notify all clients in chat room about the message
                std::vector<uint8_t> messagePacket;
                PacketWriter writer(messagePacket);
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

bool RelayIMServer::GetIsRunning() const
{
    return m_running;
}
