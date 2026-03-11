#include "ClientNetworkInterface.h"
#include "Util.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

bool ClientNetworkInterface::Initialize()
{
    WSADATA wsaData;
    int wsaStartupError = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaStartupError) {
        PrintWSAError("WSAStartup failed");
        return false;
    }

    std::cout << "WSAStartup succeeded. Version: " << (wsaData.wVersion >> 8) << "." << (wsaData.wVersion & 0xFF) << std::endl;

    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    int iResult = getaddrinfo(DEFAULT_ADDRESS, DEFAULT_PORT, &hints, &m_addrInfo);
    if (iResult != 0) {
        PrintWSAError("getaddrinfo failed");
        WSACleanup();
        return false;
    }

    // Create a SOCKET for connecting to server
    m_clientSocket = socket(m_addrInfo->ai_family, m_addrInfo->ai_socktype, m_addrInfo->ai_protocol);

    if (m_clientSocket == INVALID_SOCKET) {
        PrintWSAError("Error at socket()");
        freeaddrinfo(m_addrInfo);
        WSACleanup();
        return false;
    }

    m_isInitialized = true;
    return true;
}

bool ClientNetworkInterface::Connect()
{
    // Connect to server.
    int connectResult = connect(m_clientSocket, m_addrInfo->ai_addr, (int)m_addrInfo->ai_addrlen);
    if (connectResult == SOCKET_ERROR) {

        // If we couldn't connect, keep the socket alive in case we forgot to turn on the server or something.

        return false;
    }

    m_running = true;
    m_receiveThread = std::thread(&ClientNetworkInterface::ReceiveLoop, this);

    return true;
}

void ClientNetworkInterface::ReceiveLoop()
{
    uint8_t receiveBuffer[NETWORK_BUFLEN];

    while (m_running)
    {
        memset(receiveBuffer, 0, NETWORK_BUFLEN);

        int recvResult = recv(m_clientSocket, (char*)receiveBuffer, NETWORK_BUFLEN, 0); // Thread blocks here until data is received or the connection is closed
        if (recvResult == 0)
        {
            std::cout << "Server disconnected" << std::endl;

            if (OnServerDisconnected) {
                OnServerDisconnected();
            }

            break;
        }
        else if (recvResult == SOCKET_ERROR)
        {
            PrintWSAError("recv failed");

            if (OnServerDisconnected) {
                OnServerDisconnected();
            }

            break;
        }

        LogDepthConditional(LOG_NETWORK_BYTESTREAM, 0, "Received data from server: %u bytes\n", recvResult);
        m_receiveBuffer.insert(m_receiveBuffer.end(), receiveBuffer, receiveBuffer + recvResult);

        /////////////////////////////////////////////
        // PROCESS ACCUMULATED DATA INTO PACKETS
        /////////////////////////////////////////////

        // TODO(Salads): Send and Receive in Network Byte-Order (Big Endian), then convert on host.
        while (true) // We might receive multiple packets in one recv.
        {
            size_t receivedDataSize = m_receiveBuffer.size();
            if (receivedDataSize < 2) // Need packet size to continue.
            {
                break;
            }

            uint16_t packetSize;
            memcpy(&packetSize, m_receiveBuffer.data(), 2);

            if (receivedDataSize >= packetSize) // We have a full packet in the buffer
            {
                // Insert to packet, then remove from receive buffer.
                std::unique_ptr<PacketData> newPacketData = std::make_unique<PacketData>();
                newPacketData->insert(newPacketData->end(), m_receiveBuffer.begin(), m_receiveBuffer.begin() + packetSize);
                m_receiveBuffer.erase(m_receiveBuffer.begin(), m_receiveBuffer.begin() + packetSize);

                if (OnPacketReceived)
                {
                    // Clients don't need to know the peer id, since we know incoming traffic is from the server.
                    // We're just reusing the NetworkPacket class here in the client, with a junk peer id.
                    std::unique_ptr<NetworkPacket> newPacket = std::make_unique<NetworkPacket>(INVALID_PEER_ID, std::move(newPacketData));
                    OnPacketReceived(std::move(newPacket));
                }
            }
            else
            {
                break; // We don't have a full packet yet, wait for more data.
            }
        }

    } // while(m_running)
}

void ClientNetworkInterface::Shutdown()
{
    m_running = false;

    if (m_clientSocket != INVALID_SOCKET) {
        shutdown(m_clientSocket, SD_BOTH);
        closesocket(m_clientSocket);
        m_clientSocket = INVALID_SOCKET;
    }

    if (m_receiveThread.joinable())
    {
        m_receiveThread.join();
    }

    freeaddrinfo(m_addrInfo);
    WSACleanup();
    m_isInitialized = false;
}

void ClientNetworkInterface::Send(PacketData& data)
{
    if (!m_isInitialized)
    {
        printf("ClientNetworkInterface not initialized. Cannot send data.\n");
        return;
    }

    int iResult = send(m_clientSocket, reinterpret_cast<const char*>(data.data()), static_cast<int>(data.size()), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
    }

    LogDepthConditional(LOG_NETWORK_BYTESTREAM, 0, "Sent data to server: %u bytes\n", iResult);
}