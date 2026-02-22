#include <iostream>
#include "ServerNetworkInterface.h"

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
    if (m_listenSocket != INVALID_SOCKET)
    {
        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
    }
    
    if (m_listenSocketInfo)
    {
        freeaddrinfo(m_listenSocketInfo);
        m_listenSocketInfo = nullptr;
    }

    for (auto &[peerID, peerClient] : m_peerClients)
    {
        if (peerClient->m_receiveThread.joinable())
        {
            peerClient->m_receiveThread.join();
        }

        shutdown(peerClient->m_clientSocket, SD_BOTH);
        closesocket(peerClient->m_clientSocket);
    }

    WSACleanup();
}

void ServerNetworkInterface::ListenForClients()
{
    while (true)
    {
        SOCKET newClientSocket = INVALID_SOCKET;
        newClientSocket = accept(m_listenSocket, NULL, NULL); // blocking
        if (newClientSocket == INVALID_SOCKET) {
            PrintWSAError("accept failed");
            return;
        }

        std::cout << "New client connected: " << m_nextClientID << std::endl;
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
            std::cout << "Client " << client->m_peerID << " disconnected" << std::endl;

            if (OnClientDisconnected) {
                OnClientDisconnected(client->m_peerID);
            }

            break;
        }
        else if(recvResult == SOCKET_ERROR)
        {
            PrintWSAError("recv failed");
            break;
        }

        std::cout << "Received data from client " << client->m_peerID << ": " << recvResult << " bytes" << std::endl;
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
                std::vector<uint8_t> packet;
                packet.insert(packet.end(), client->m_receiveBuffer.begin(), client->m_receiveBuffer.begin() + packetSize);
                client->m_receiveBuffer.erase(client->m_receiveBuffer.begin(), client->m_receiveBuffer.begin() + packetSize);

                if (OnPacketReceived)
                {
                    OnPacketReceived(client->m_peerID, &packet);
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
        std::unique_lock<std::mutex> lock(client->m_sendThreadCVMutex);
        client->m_sendThreadCV.wait(lock, [this, client]() {
            std::lock_guard<std::mutex> sendBufferLock(client->m_sendBufferMutex);
            return !client->m_sendBuffer.empty();
        });

        {
            std::lock_guard<std::mutex> sendBufferLock(client->m_sendBufferMutex);
            int sendResult = send(peerSocket, (char*)client->m_sendBuffer.data(), (int)client->m_sendBuffer.size(), 0);
            if (sendResult == SOCKET_ERROR)
            {
                PrintWSAError("send failed");
            }
            else
            {
                std::cout << "Sent data to client " << client->m_peerID << ": " << sendResult << " bytes" << std::endl;
                client->m_sendBuffer.erase(client->m_sendBuffer.begin(), client->m_sendBuffer.begin() + sendResult);
            }
        }
    }
}

void ServerNetworkInterface::SendToClient(PeerID clientPeerID, std::vector<uint8_t>* packet)
{
    *reinterpret_cast<uint16_t*>(packet->data()) = static_cast<uint16_t>(packet->size()); // Update packet size in header

    std::lock_guard<std::mutex> lock(m_peerClientsMutex);
    m_peerClients[clientPeerID]->Send(packet);
}