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

    struct addrinfo * result = NULL,
                    * ptr = NULL,
                    hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    int iResult = getaddrinfo(DEFAULT_ADDRESS, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        PrintWSAError("getaddrinfo failed");
        WSACleanup();
        return false;
    }

    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr = result;

    // Create a SOCKET for connecting to server
    m_clientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (m_clientSocket == INVALID_SOCKET) {
        PrintWSAError("Error at socket()");
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    // Connect to server.
    iResult = connect(m_clientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(m_clientSocket);
        m_clientSocket = INVALID_SOCKET;
        return false;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (m_clientSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return false;
    }

    m_isInitialized = true;
    m_running = true;
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
            break;
        }

        std::cout << "Received data from server: " << recvResult << " bytes" << std::endl;
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
    if (m_clientSocket != INVALID_SOCKET) {
        closesocket(m_clientSocket);
        m_clientSocket = INVALID_SOCKET;
    }

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

    printf("Bytes Sent: %ld\n", iResult);
}