#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>

#include "Util.h"
#include "NetworkPeer.h"
#include "Types.h"
#include "ChatClient.h"
#include "PeerClient.h"

class ServerPeer : public NetworkPeer
{
public:
    bool Initialize() override;
    void Shutdown() override;

    void ListenForClients();
    void UpdateNetworkForPeer(PeerID peerID, SOCKET peerSocket);

    std::function<void(PeerID)> OnNewClient;

private:
    addrinfo* m_listenSocketInfo = nullptr;
    SOCKET m_listenSocket = INVALID_SOCKET;

    std::thread m_listenThread;

    std::atomic_uint32_t m_nextClientID{ 0 };
    std::unordered_map<PeerID, std::unique_ptr<PeerClient>> m_peerClients;
    std::mutex m_peerClientsMutex;
};