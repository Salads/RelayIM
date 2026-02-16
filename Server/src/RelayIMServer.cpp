#include "RelayIMServer.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#include "Util.h"
#include "PacketType.h"
#include "NetworkTypes.h"

bool RelayIMServer::Initialize()
{
    if (m_isInitialized) { return false; }

    if (!m_serverPeer.Initialize())
    {
        std::cerr << "Failed to initialize server peer" << std::endl;
        return false;
    }

    std::cout << "Server Initialized" << std::endl;
    m_isInitialized = true;
    return true;
}

void RelayIMServer::Stop()
{
    m_serverPeer.Shutdown();
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
    }
    
    // TODO(Salads): Receive data from clients and handle it.

    return true;
}
