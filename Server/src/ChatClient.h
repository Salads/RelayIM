#pragma once

#include <WinSock2.h>
#include <cstdint>

class ChatClient
{
public:
    SOCKET m_clientSocket;
    uint32_t m_clientID;
};