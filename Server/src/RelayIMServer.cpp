#include "RelayIMServer.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#include "Util.h"
#include "PacketType.h"
#include "PacketReader.h"
#include "PacketWriter.h"
#include "Logging.h"

RelayIMServer::RelayIMServer()
    : m_serverNetwork(this)
{}

bool RelayIMServer::initializeServer()
{
    if (isInitialized()) { return true; }

    if (!m_serverNetwork.initializeInterface())
    {
        std::cerr << "Failed to initialize server network interface..." << std::endl;
        return false;
    }

    std::cout << "Server Initialized" << std::endl;
    m_isInitialized = true;
    return true;
}

void RelayIMServer::onNewClient(PeerID newPeerID)
{
    handleNewClient(newPeerID);
}

void RelayIMServer::onClientDisconnected(PeerID peerID)
{
    std::lock_guard<std::mutex> lock(m_clientsMutex);
    m_clients.erase(peerID);
}

void RelayIMServer::onPacketReceived(PeerID peerID, std::unique_ptr<NetworkPacket> packet)
{
    this->addPacketToQueue(std::move(packet));
}

bool RelayIMServer::start()
{
    if (!isInitialized())
    {
        initializeServer();

        if (!isInitialized())
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
    
    m_packetHandlerThread = std::thread(&RelayIMServer::processClientPackets, this);
    m_running = true;

    std::cout << "Server started!" << std::endl;
    return true;
}

void RelayIMServer::stop()
{
    m_running = false;
    m_incomingPacketsCV.notify_one();

    std::cout << "Shutting down server network interface..." << std::endl;
    m_serverNetwork.shutdownInterface();

    std::cout << "Cleaning up already disconnected clients" << std::endl;
    m_serverNetwork.deleteDisconnectedClients();

    std::cout << "Joining packet handler thread..." << std::endl;
    if (m_packetHandlerThread.joinable())
    {
        m_packetHandlerThread.join();
    }

    std::cout << "Stopped RelayIMServer" << std::endl;
}

void RelayIMServer::update()
{
    m_serverNetwork.deleteDisconnectedClients();
}

void RelayIMServer::processClientPackets()
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

        PeerID peerID = packet.get()->m_peerId;
        PacketHeader header;
        if (!reader.readHeader(header))
        {
            continue;
        }

        Log::get()->conditionalWriteLine(LOG_NETWORK_PACKET_TYPES, "RECV(%s) from Peer(%u)", packetTypeToString(header.m_packetType), peerID);

        // Read Packet Payload
        switch (header.m_packetType)
        {
        case PacketType_Connect:
        {
            std::string newUsername;
            if (!reader.readString(newUsername))
            {
                break;
            }

            if (!isUsernameTaken(newUsername))
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
                writer.writeHeader(PacketType_ConnectResponse);
                writer.writeUInt8(PacketResponseReason::Success);
                writer.writePeerId(peerID);
                writer.writeString(newUsername);
                writer.finalize();
                m_serverNetwork.sendToClient(peerID, &response);
            }
            else
            {
                PacketData response;
                PacketWriter writer(response);
                writer.writeHeader(PacketType_ConnectResponse);
                writer.writeUInt8(PacketResponseReason::UsernameTaken);
                writer.finalize();
                m_serverNetwork.sendToClient(peerID, &response);
            }

            break;
        }
        case PacketType_ListChatRooms:
        {
            // Send PacketType_ListChatRooms_Result -> ARRAY[RoomID, RoomName]
            PacketData response;
            PacketWriter writer(response);
            writer.writeHeader(PacketType_ListChatRooms_Result);
            writer.writeUInt16(static_cast<uint16_t>(m_chatRooms.size()));

            for(const auto& [roomID, chatRoom] : m_chatRooms)
            {
                writer.writeRoomId(roomID);

                std::string roomName = chatRoom->getRoomName();
                writer.writeString(roomName);
            }

            writer.finalize();

            m_serverNetwork.sendToClient(peerID, &response);
        } break;
        case PacketType_JoinChatRoom:
        {
            RoomID roomID;
            if (!reader.readRoomID(roomID))
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
                joinerResponseWriter.writeHeader(PacketType_JoinChatRoomResponse);
                joinerResponseWriter.writeUInt8(PacketResponseReason::Success);
                joinerResponseWriter.writeRoomId(roomID);
                joinerResponseWriter.writeString(m_chatRooms[roomID]->getRoomName());
                joinerResponseWriter.finalize();
                m_serverNetwork.sendToClient(peerID, &joinerResponse);

                // Notify all clients (except joiner) of this chat room of the new client
                std::vector<uint8_t> newUserResponse;
                PacketWriter newUserWriter(newUserResponse);
                newUserWriter.writeHeader(PacketType_RoomUpdate_UserJoined);
                newUserWriter.writeRoomId(roomID);
                newUserWriter.writePeerId(peerID);
                newUserWriter.writeString(joiningUsername);
                newUserWriter.finalize();

                std::vector<PeerID> roomClients = m_chatRooms[roomID]->getClients();
                for(PeerID roomClient : roomClients)
                {
                    m_serverNetwork.sendToClient(roomClient, &newUserResponse);
                }

                // Send FULL update to newly joined client (arr users, arr messages)
                std::vector<uint8_t> fullResponse;
                PacketWriter fullWriter(fullResponse);
                fullWriter.writeHeader(PacketType_RoomUpdate_FULL);
                fullWriter.writeRoomId(roomID);
                fullWriter.writeUInt16(static_cast<uint16_t>(roomClients.size()));

                {
                    std::lock_guard lock(m_clientsMutex);
                    for(PeerID roomClient : roomClients)
                    {
                        fullWriter.writePeerId(roomClient);
                        fullWriter.writeString(m_clients[peerID].get()->m_username);
                    }
                }

                ChatRoom* chatRoom = m_chatRooms[roomID].get();
                const std::vector<ChatMessage>* messages = chatRoom->getMessages();

                uint16_t nMessages = static_cast<uint16_t>(messages->size());
                fullWriter.writeUInt16(nMessages);

                for(int i = 0; i < nMessages; i++)
                {
                    const ChatMessage& message = messages->at(i);
                    fullWriter.writePeerId(message.m_senderId);
                    fullWriter.writeString(message.m_message);
                }

                fullWriter.finalize();
                m_serverNetwork.sendToClient(peerID, &fullResponse);
                m_chatRooms[roomID]->addClient(peerID);

            }
            else
            {
                PacketData response;
                PacketWriter writer(response);
                writer.writeHeader(PacketType_JoinChatRoomResponse);
                writer.writeUInt8(PacketResponseReason::ChatRoomDoesntExist);
                writer.finalize();
                m_serverNetwork.sendToClient(peerID, &response);
            }

            break;
        }
        case PacketType_CreateChatRoom:
        {
            std::string roomName;
            if (!reader.readString(roomName))
            {
                break;
            }

            if (!isRoomnameTaken(roomName))
            {
                RoomID newRoomID(m_nextRoomID++);
                std::unique_ptr<ChatRoom> newChatRoom = std::make_unique<ChatRoom>(newRoomID, roomName);
                m_chatRooms[newRoomID] = std::move(newChatRoom);
                m_chatRooms[newRoomID]->addClient(peerID);

                PacketData response;
                PacketWriter writer(response);
                writer.writeHeader(PacketType_CreateChatRoomResponse);
                writer.writeUInt8(PacketResponseReason::Success);
                writer.writeRoomId(newRoomID);
                writer.writeString(roomName);
                writer.finalize();
                m_serverNetwork.sendToClient(peerID, &response);

            }
            else
            {
                PacketData response;
                PacketWriter writer(response);
                writer.writeHeader(PacketType_CreateChatRoomResponse);
                writer.writeUInt8(PacketResponseReason::ChatRoomNameTaken);
                writer.finalize();
                m_serverNetwork.sendToClient(peerID, &response);
            }

            break;
        }
        case PacketType_LeaveChatRoom:
        {
            RoomID roomID;
            if (!reader.readRoomID(roomID))
            {
                break;
            }

            if (m_chatRooms.contains(roomID))
            {
                // Notify chat room members of leaving user
                std::vector<uint8_t> responsePacket;
                PacketWriter writer(responsePacket);
                writer.writeHeader(PacketType_RoomUpdate_UserLeft);
                writer.writeRoomId(roomID);
                writer.writePeerId(peerID);
                writer.finalize();

                std::vector<PeerID> roomClients = m_chatRooms[roomID]->getClients();
                for(PeerID roomClient : roomClients)
                {
                    m_serverNetwork.sendToClient(roomClient, &responsePacket);
                }

                m_chatRooms[roomID]->removeClient(peerID);
            }

            break;
        }
        case PacketType_SendMessage:
        {
            RoomID roomID;
            if (!reader.readRoomID(roomID))
            {
                break;
            }

            std::string message;
            if (!reader.readString(message))
            {
                break;
            }

            if (m_chatRooms.contains(roomID))
            {
                ChatMessage newMessage(peerID, message);

                // Add message to chat room, get all clients in chat room
                std::vector<PeerID> chatRoomClients = m_chatRooms[roomID]->getClients();
                m_chatRooms[roomID]->addMessage(newMessage);

                // Notify all clients in chat room about the message
                std::vector<uint8_t> messagePacket;
                PacketWriter writer(messagePacket);
                writer.writeHeader(PacketType_RoomUpdate_MSG);
                writer.writeRoomId(roomID);
                writer.writePeerId(peerID);
                writer.writeString(message);
                writer.finalize();

                for (PeerID chatRoomClientID : chatRoomClients)
                {
                    m_serverNetwork.sendToClient(chatRoomClientID, &messagePacket);
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

void RelayIMServer::addPacketToQueue(std::unique_ptr<NetworkPacket> newPacket)
{
    std::lock_guard lock(m_incomingPacketsMutex);
    m_incomingPackets.push(std::move(newPacket));
    m_incomingPacketsCV.notify_one();
}

bool RelayIMServer::isRoomnameTaken(std::string& newRoomname)
{
    for (auto &[roomID, chatRoom] : m_chatRooms)
    {
        if (chatRoom->getRoomName() == newRoomname)
        {
            return true;
        }
    }

    return false;
}

bool RelayIMServer::isUsernameTaken(std::string &newUsername)
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

void RelayIMServer::handleNewClient(PeerID newPeerID)
{
    std::unique_ptr<ChatClient> newChatClient = std::make_unique<ChatClient>(newPeerID);
    {
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        m_clients.emplace(newPeerID, std::move(newChatClient));
    }
}

bool RelayIMServer::isInitialized() const
{
    return m_isInitialized;
}

bool RelayIMServer::getIsRunning() const
{
    return m_running;
}
