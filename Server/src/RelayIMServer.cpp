#include "RelayIMServer.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#include "Util.h"
#include "PacketType.h"

#define DEFAULT_PORT "27015"

bool RelayIMServer::Initialize()
{
    if (m_isInitialized) { return false; }

    WSADATA wsaData;
    int wsaStartupError = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaStartupError) {
        PrintWSAError("WSAStartup failed");
        return 1;
    }

    std::cout << "WSAStartup succeeded. Version: " << (wsaData.wVersion >> 8) << "." << (wsaData.wVersion & 0xFF) << std::endl;

    addrinfo hints = { 0 }; // Type of sockets we support

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

    std::cout << "Server Initialized" << std::endl;
    m_isInitialized = true;
    return true;
}

void RelayIMServer::Stop()
{
    if (m_listenThread.joinable())
    {
        m_listenThread.join();
    }

    {
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        for (auto& [clientID, client] : m_clients)
        {
            if (client->m_thread.joinable())
            {
                client->m_thread.join();
            }
            closesocket(client->m_clientSocket);
        }
    }

    shutdown(m_listenSocket, SD_BOTH);
    freeaddrinfo(m_listenSocketInfo);
    WSACleanup();
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

        m_listenThread = std::thread(&RelayIMServer::ListenForClients, this);
    }
    
    // TODO(Salads): Receive data from clients and handle it.

    return true;
}

void RelayIMServer::ListenForClients()
{
    while (true)
    {
        SOCKET newClientSocket = INVALID_SOCKET;
        newClientSocket = accept(m_listenSocket, NULL, NULL);
        if (newClientSocket == INVALID_SOCKET) {
            PrintWSAError("accept failed");
            return;
        }

        std::cout << "New client connected: " << newClientSocket << std::endl;
        ChatClient* newClient = std::make_unique<ChatClient>(newClientSocket, m_nextClientID++).release();
        newClient->m_thread = std::thread(&RelayIMServer::ProcessClient, this, newClient);
        {
            std::lock_guard<std::mutex> lock(m_clientsMutex);
            m_clients.emplace(newClient->m_clientID, newClient);
        }
    }
}

#define DEFAULT_BUFLEN 512
#define BUFLEN DEFAULT_BUFLEN

void RelayIMServer::ProcessClient(ChatClient *client)
{
    uint8_t receiveBuffer[BUFLEN];

    int recvResult = recv(client->m_clientSocket, (char*)receiveBuffer, BUFLEN, 0);
    if (recvResult > 0)
    {
        std::cout << "Received data from client " << client->m_clientID << ": " << recvResult << " bytes" << std::endl;
        // TODO(Salads): Handle the received data.
    }
    else if (recvResult == 0)
    {
        std::cout << "Client " << client->m_clientID << " disconnected" << std::endl;
    }
    else
    {
        PrintWSAError("recv failed");
    }
}