#pragma once

#include <WinSock2.h>
#include <cstdint>
#include <thread>

#include "ChatClientStatus.h"

#define INVALID_CLIENT_ID -1

class ChatClient
{
public:
    ChatClient(SOCKET clientSocket, uint32_t clientID)
        : m_clientSocket(clientSocket), m_clientID(clientID) {}
public:
    SOCKET m_clientSocket = INVALID_SOCKET;
    ClientID m_clientID = INVALID_CLIENT_ID;

    std::thread m_thread;

    ChatClientStatus m_status = ChatClientStatus_Fresh;
};