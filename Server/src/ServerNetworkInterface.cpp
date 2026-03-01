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

    // This is proper, but OS cleans things up for us... are we slowing things down for no reason?
    for (auto &[peerID, peerClient] : m_peerClients)
    {
        shutdown(peerClient->m_clientSocket, SD_BOTH);
        closesocket(peerClient->m_clientSocket);

        if (peerClient->m_receiveThread.joinable())
        {
            peerClient->m_receiveThread.join();
        }

        if (peerClient->m_sendThread.joinable())
        {
            peerClient->m_sendThread.join();
        }
    }

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

        LogDepth(0, "New client socket connected: %u\n", m_nextClientID.load());
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
        memset(receiveBuffer, 0, NETWORK_BUFLEN);

        int recvResult = recv(peerSocket, (char*)receiveBuffer, NETWORK_BUFLEN, 0); // Thread blocks here until data is received or the connection is closed
        if (recvResult == 0)
        {
            LogDepth(0, "Client %u disconnected\n", client->m_peerID);

            if (OnClientDisconnected) {
                OnClientDisconnected(client->m_peerID);
            }

            break;
        }
        else if(recvResult == SOCKET_ERROR) // Client disconnected forcibly, or some Winsock2 error. Either way, client should be disconnected.
        {
            PrintWSAError("recv failed");

            if (OnClientDisconnected) {
                OnClientDisconnected(client->m_peerID);
            }

            break;
        }

        LogDepthConditional(LOG_NETWORK_BYTESTREAM, 0, "Received data from client %u: %u bytes\n", client->m_peerID, recvResult);
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

void ServerNetworkInterface::SendLoopForClient(PeerClient* client, SOCKET peerSocket)
{
    while (m_running)
    {
        // Wait untill we have data to send
        std::unique_lock<std::mutex> lock(client->m_sendBufferMutex);
        client->m_sendThreadCV.wait(lock, [this, client]() {
            return !m_running || !client->m_sendBuffer.empty();
        });

        if (!m_running)
        {
            lock.unlock();
            break;
        }

        int sendResult = send(peerSocket, (char*)client->m_sendBuffer.data(), (int)client->m_sendBuffer.size(), 0);
        if (sendResult == SOCKET_ERROR)
        {
            PrintWSAError("send failed");
        }
        else
        {
            LogDepthConditional(LOG_NETWORK_BYTESTREAM, 0, "Sent data to client %u: %u bytes\n", client->m_peerID, sendResult);
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
        client->Send(packet);
    }
    else
    {
        LogDepth(0, "SendToCLient could not find client %u\n", clientPeerID);
    }
}

