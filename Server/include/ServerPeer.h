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
    void UpdateNetworkForPeer(PeerClient* client, SOCKET peerSocket);

    std::function<void(PeerID)> OnNewClient;
    std::function<void(PeerID, std::vector<uint8_t> *packet)> OnPacketReceived;
    std::function<void(PeerID)> OnClientDisconnected;

private:
    addrinfo* m_listenSocketInfo = nullptr;
    SOCKET m_listenSocket = INVALID_SOCKET;

    std::atomic_bool m_running = false;

    std::thread m_listenThread;

    std::atomic_uint32_t m_nextClientID{ 0 };
    std::unordered_map<PeerID, std::unique_ptr<PeerClient>> m_peerClients;
    std::mutex m_peerClientsMutex;
};