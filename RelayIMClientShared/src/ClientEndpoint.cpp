#include "pch.h"
#include "ClientEndpoint.h"
#include "Util.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

bool ClientEndpoint::Initialize()
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
    return true;
}

void ClientEndpoint::Shutdown()
{
    if (m_clientSocket != INVALID_SOCKET) {
        closesocket(m_clientSocket);
        m_clientSocket = INVALID_SOCKET;
    }

    WSACleanup();
    m_isInitialized = false;
}

void ClientEndpoint::Send(std::vector<uint8_t> &data)
{
    if (!m_isInitialized)
    {
        printf("ClientEndpoint not initialized. Cannot send data.\n");
        return;
    }

    // TODO(Salads): Instead of trying to precog the packet size, insert it here instead. Resize the vector beforehand and just edit to avoid vector shifting O(n).
    int iResult = send(m_clientSocket, reinterpret_cast<const char*>(data.data()), data.size(), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
    }

    printf("Bytes Sent: %ld\n", iResult);
}