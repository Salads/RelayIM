#pragma once

#include <WinSock2.h>

class RelayIMServer
{
public:
    bool Initialize();
    bool Run();
    void Stop();

    bool IsInitialized() const;

private:
    bool m_isInitialized = false;

    addrinfo* m_listenSocketInfo = nullptr;
    SOCKET m_listenSocket = INVALID_SOCKET;
};