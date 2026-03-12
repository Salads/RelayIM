#include <iostream>
#include "ServerNetworkInterface.h"
#include "Util.h"
#include "Logging.h"

using std::cout;
using std::endl;

bool ServerNetworkInterface::Initialize()
{
    WSADATA wsaData;
    int wsaStartupError = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaStartupError) {
        PrintWSAError("WSAStartup failed");
        return false;
    }

    std::cout << "WSAStartup succeeded. Version: " << (wsaData.wVersion >> 8) << "." << (wsaData.wVersion & 0xFF) << std::endl;

    addrinfo hints = { 0 }; // Kind of socket we want.

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_flags = AI_PASSIVE; // For servers
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_protocol = IPPROTO_TCP;

    // Compatability translation between desired and actual support
    int getadderinfoError = getaddrinfo(NULL, DEFAULT_PORT, &hints, &m_listenSocketInfo);
    if (getadderinfoError) {
        PrintWSAError("getaddrinfo failed");
        WSACleanup();
        return false;
    }

    // Closest socket we can get to the desired socket
    m_listenSocket = socket(m_listenSocketInfo->ai_family, m_listenSocketInfo->ai_socktype, m_listenSocketInfo->ai_protocol);
    if (m_listenSocket == INVALID_SOCKET) {
        PrintWSAError("socket failed");
        freeaddrinfo(m_listenSocketInfo);
        WSACleanup();
        return false;
    }

    // Setup the TCP listening socket
    int bindError = bind(m_listenSocket, m_listenSocketInfo->ai_addr, (int)m_listenSocketInfo->ai_addrlen);
    if (bindError == SOCKET_ERROR) {
        PrintWSAError("bind failed");
        freeaddrinfo(m_listenSocketInfo);
        closesocket(m_listenSocket);
        WSACleanup();
        return false;
    }

    // Start listening on the socket. This is how the server will actually know about incoming data.
    if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        PrintWSAError("listen failed");
        freeaddrinfo(m_listenSocketInfo);
        closesocket(m_listenSocket);
        WSACleanup();
        return false;
    }

    m_running = true;
    m_listenThread = std::thread(&ServerNetworkInterface::ListenForClients, this);
    m_isInitialized = true;

    return true;
}

void ServerNetworkInterface::Shutdown()
{
    m_running = false;

    if (m_listenSocket != INVALID_SOCKET)
    {
        shutdown(m_listenSocket, SD_BOTH);
        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
    }

    if (m_listenThread.joinable())
    {
        m_listenThread.join();
    }
    
    if (m_listenSocketInfo)
    {
        freeaddrinfo(m_listenSocketInfo);
        m_listenSocketInfo = nullptr;
    }

    m_peerClients.clear();

    WSACleanup();
}

void ServerNetworkInterface::ListenForClients()
{
    while (m_running)
    {
        SOCKET newClientSocket = INVALID_SOCKET;
        newClientSocket = accept(m_listenSocket, NULL, NULL); // blocking
        if (newClientSocket == INVALID_SOCKET) {
            PrintWSAError("accept failed");
            return;
        }

        std::unique_ptr<PeerClient> newPeerClient = std::make_unique<PeerClient>(m_nextClientID++, newClientSocket);
        newPeerClient->m_receiveThread = std::thread(&ServerNetworkInterface::ReceiveLoopForClient, this, newPeerClient.get(), newClientSocket);
        newPeerClient->m_sendThread = std::thread(&ServerNetworkInterface::SendLoopForClient, this, newPeerClient.get(), newClientSocket);
        {
            std::lock_guard<std::mutex> lock(m_peerClientsMutex);
            PeerID newClientID = newPeerClient->m_peerID;

            m_peerClients.emplace(newClientID, std::move(newPeerClient));

            if (OnNewClient) {
                OnNewClient(newClientID);
            }
        }
    }
}

void ServerNetworkInterface::ReceiveLoopForClient(PeerClient *client, SOCKET peerSocket)
{
    uint8_t receiveBuffer[NETWORK_BUFLEN];

    while (m_running)
    {
        if (client->GetMarkedForDeletion())
        {
            break;
        }

        memset(receiveBuffer, 0, NETWORK_BUFLEN);

        int recvResult = recv(peerSocket, (char*)receiveBuffer, NETWORK_BUFLEN, 0); // Thread blocks here until data is received or the connection is closed
        if (recvResult == 0)
        {
            MarkPeerClientForDeletion(client->m_peerID);

            if (OnClientDisconnected) 
            {
                OnClientDisconnected(client->m_peerID);
            }

            break;
        }
        else if(recvResult == SOCKET_ERROR) // Client disconnected forcibly, or some Winsock2 error. Either way, client should be disconnected.
        {
            PrintWSAError("recv failed");

            MarkPeerClientForDeletion(client->m_peerID);

            if (OnClientDisconnected) 
            {
                OnClientDisconnected(client->m_peerID);
            }

            break;
        }

        client->m_receiveBuffer.insert(client->m_receiveBuffer.end(), receiveBuffer, receiveBuffer + recvResult);

        /////////////////////////////////////////////
        // PROCESS ACCUMULATED DATA INTO PACKETS
        /////////////////////////////////////////////

        // TODO(Salads): Send and Receive in Network Byte-Order (Big Endian), then convert on host.
        while (true) // We might receive multiple packets in one recv.
        {
            size_t receivedDataSize = client->m_receiveBuffer.size();
            if (receivedDataSize < 2) // Need packet size to continue.
            {
                break;
            }

            uint16_t packetSize;
            memcpy(&packetSize, client->m_receiveBuffer.data(), 2);

            if (receivedDataSize >= packetSize) // We have a full packet in the buffer
            {
                // Insert to packet, then remove from receive buffer.
                std::unique_ptr<PacketData> newPacketData = std::make_unique<PacketData>();
                newPacketData->insert(newPacketData->end(), client->m_receiveBuffer.begin(), client->m_receiveBuffer.begin() + packetSize);
                client->m_receiveBuffer.erase(client->m_receiveBuffer.begin(), client->m_receiveBuffer.begin() + packetSize);

                if (OnPacketReceived)
                {
                    std::unique_ptr<NetworkPacket> newPacket = std::make_unique<NetworkPacket>(client->m_peerID, std::move(newPacketData));
                    OnPacketReceived(client->m_peerID, std::move(newPacket));
                }
            }
            else
            {
                break; // We don't have a full packet yet, wait for more data.
            }
        }

    } // while(m_running)
}

void ServerNetworkInterface::DeleteDisconnectedClients()
{
    {
        std::lock_guard lock(m_deletedPeerClientsMutex);
        int nDeleted = 0;
        while (!m_deletedPeerClients.empty())
        {
            m_deletedPeerClients.pop();
        }

    }
}

void ServerNetworkInterface::MarkPeerClientForDeletion(PeerID peerID)
{
    {
        std::lock(m_deletedPeerClientsMutex, m_peerClientsMutex);
        std::lock_guard lk1(m_deletedPeerClientsMutex, std::adopt_lock);
        std::lock_guard lk2(m_peerClientsMutex, std::adopt_lock);

        std::unique_ptr<PeerClient> deletedClient = std::move(m_peerClients[peerID]);
        deletedClient->MarkForDeletion(true);
        deletedClient->m_sendThreadCV.notify_one();
        m_peerClients.erase(peerID);
        m_deletedPeerClients.push(std::move(deletedClient));
    }
}

void ServerNetworkInterface::SendLoopForClient(PeerClient* client, SOCKET peerSocket)
{
    while (m_running && !client->GetMarkedForDeletion())
    {
        // Wait untill we have data to send
        std::unique_lock<std::mutex> lock(client->m_sendBufferMutex);
        client->m_sendThreadCV.wait(lock, [this, client]() {
            return !m_running || !client->m_sendBuffer.empty() || client->GetMarkedForDeletion();
        });

        if (!m_running || client->GetMarkedForDeletion())
        {
            break;
        }

        int sendResult = send(peerSocket, (char*)client->m_sendBuffer.data(), (int)client->m_sendBuffer.size(), 0);
        if (sendResult == SOCKET_ERROR)
        {
            PrintWSAError("send failed");
        }
        else
        {
            client->m_sendBuffer.erase(client->m_sendBuffer.begin(), client->m_sendBuffer.begin() + sendResult);
        }
    }
}

void ServerNetworkInterface::SendToClient(PeerID clientPeerID, PacketData* packet)
{
    PeerClient* client;
    {
        std::lock_guard<std::mutex> lock(m_peerClientsMutex);
        client = m_peerClients[clientPeerID].get();
    }

    if (client)
    {
        uint8_t* type = packet->data() + 7; // 8th Byte is packet type
        Log::Get()->ConditionalWriteLine(LOG_NETWORK_PACKET_TYPES, "SEND(%s) to Peer(%u)", PacketTypeToString(*type), clientPeerID);
        client->Send(packet);
    }
}

