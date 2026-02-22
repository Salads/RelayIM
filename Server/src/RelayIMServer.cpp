#include "RelayIMServer.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#include "Util.h"
#include "PacketType.h"
#include "NetworkTypes.h"
#include "BinaryReader.h"

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
        // TODO(Salads): Update server state from client packets
        case PacketType_JoinChatRoom:
        {
            uint32_t roomID = 0; reader.ReadUInt32(roomID);
            uint8_t create = 0; reader.ReadUInt8(create);
            std::string roomName; reader.ReadString(roomName);
            std::cout << "Client " << peerID << " wants to join chat room " << roomID << " (Create: " << (int)create << ", Room Name: " << roomName << ")" << std::endl;
            break;
        }
        case PacketType_LeaveChatRoom:
        {
            uint32_t roomID = 0; reader.ReadUInt32(roomID);
            std::cout << "Client " << peerID << " wants to leave chat room " << roomID << std::endl;
            break;
        }
        case PacketType_SendMessage:
        {
            uint32_t roomID = 0; reader.ReadUInt32(roomID);
            std::string message; reader.ReadString(message);
            std::cout << "Client " << peerID << " wants to send message to chat room " << roomID << ": " << message << std::endl;
            break;
        }
        case PacketType_RoomUpdate:
        {
            uint32_t roomID = 0; reader.ReadUInt32(roomID);
            std::string message; reader.ReadString(message);
            std::cout << "Client " << peerID << " received room update for chat room " << roomID << ": " << message << std::endl;
            break;
        }
        default:
        {
            std::cout << "Client " << peerID << " sent unknown packet type: " << (int)packetType << std::endl;
            break;
        }
    }
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
