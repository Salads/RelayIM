#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>

#include "Util.h"
#include "NetworkInterface.h"
#include "Types.h"
#include "ChatClient.h"
#include "PeerClient.h"
#include "NetworkPacket.h"

class ServerNetworkInterface : public NetworkInterface
{
public:
    bool Initialize() override;
    void Shutdown() override;

    void ListenForClients();
    void ReceiveLoopForClient(PeerClient* client, SOCKET peerSocket);
    void SendLoopForClient(PeerClient* client, SOCKET peerSocket);

    void SendToClient(PeerID, PacketData* packet);

    std::function<void(PeerID)> OnNewClient; // New client connected (socket, no data received)
    std::function<void(PeerID, std::unique_ptr<NetworkPacket> newPacket)> OnPacketReceived; // client receive thread has constructed a new packet
    std::function<void(PeerID)> OnClientDisconnected; // socket received 0, client disconnected (socket)

private:
    addrinfo* m_listenSocketInfo = nullptr;
    SOCKET m_listenSocket = INVALID_SOCKET;

    std::atomic_bool m_running = false;

    std::thread m_listenThread;

    std::atomic_uint32_t m_nextClientID{ 0 };
    std::unordered_map<PeerID, std::unique_ptr<PeerClient>> m_peerClients;
    std::mutex m_peerClientsMutex;
};