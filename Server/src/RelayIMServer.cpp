#include "RelayIMServer.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#include "Util.h"
#include "PacketType.h"
#include "NetworkTypes.h"

bool RelayIMServer::Initialize()
{
    if (m_isInitialized) { return true; }

    if (!m_serverPeer.Initialize())
    {
        std::cerr << "Failed to initialize server peer" << std::endl;
        return false;
    }

    m_serverPeer.OnNewClient = [this](PeerID newPeerID) 
    {
        HandleNewClient(newPeerID);
    };

    std::cout << "Server Initialized" << std::endl;
    m_isInitialized = true;
    return true;
}

void RelayIMServer::HandleNewClient(PeerID newPeerID)
{
    std::unique_ptr<ChatClient> newChatClient = std::make_unique<ChatClient>(newPeerID);
    {
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        m_clients.emplace(newPeerID, std::move(newChatClient));
    }
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
