#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define DEFAULT_PORT "27015"

using std::cout;
using std::endl;

void PrintWSAError(const char* message)
{
    char buffer[256];
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        WSAGetLastError(),
        0,
        buffer,
        sizeof(buffer),
        NULL
    );
    std::cerr << message << ": " << buffer << std::endl;
}

int main()
{
    WSADATA wsaData;
    int wsaStartupError = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaStartupError) {
        PrintWSAError("WSAStartup failed");
        return 1;
    }

    std::cout << "WSAStartup succeeded. Version: " << (wsaData.wVersion >> 8) << "." << (wsaData.wVersion & 0xFF) << std::endl;

    addrinfo* result = { 0 };
    addrinfo hints = { 0 }; // Type of sockets we support

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_flags = AI_PASSIVE; // For servers
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_protocol = IPPROTO_TCP;

    // Compatability translation between desired and actual support
    int getadderinfoError = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (getadderinfoError) {
        PrintWSAError("getaddrinfo failed");
        WSACleanup();
        return 1;
    }

    // Create socket
    SOCKET listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listenSocket == INVALID_SOCKET) {
        PrintWSAError("socket failed");
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    int bindError = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (bindError == SOCKET_ERROR) {
        PrintWSAError("bind failed");
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Start listening on the socket. This is how the server will actually know about incoming data.
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        PrintWSAError("listen failed");\
        freeaddrinfo(result);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // TODO(Salads): Listener Thread
    SOCKET clientSocket = INVALID_SOCKET;

    // Accept a client socket
    clientSocket = accept(listenSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        PrintWSAError("accept failed");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    
    freeaddrinfo(result);
    WSACleanup();
    return 0;
}