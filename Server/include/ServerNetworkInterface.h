#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <queue>

#include "Util.h"
#include "NetworkInterface.h"
#include "PacketData.h"
#include "ChatClient.h"
#include "PeerClient.h"
#include "NetworkPacket.h"
#include "IServerPacketHandler.h"
#include "NetworkConfig.h"
#include "PacketType.h"

class ServerNetworkInterface : public NetworkInterface
{
public:
    ServerNetworkInterface(IServerPacketHandler* handler);

    bool Initialize() override;
    void Shutdown() override;

    void ListenForClients();
    void ReceiveLoopForClient(PeerClient* client, SOCKET peerSocket);
    void SendLoopForClient(PeerClient* client, SOCKET peerSocket);

    void SendToClient(PeerID, PacketData* packet);
    void DeleteDisconnectedClients();

private:
    void MarkPeerClientForDeletion(PeerID peerID);

private:
    addrinfo* m_listenSocketInfo = nullptr;
    SOCKET m_listenSocket = INVALID_SOCKET;

    std::atomic_bool m_running = false;

    std::thread m_listenThread;

    std::atomic_uint32_t m_nextClientID{ 0 };
    std::unordered_map<PeerID, std::unique_ptr<PeerClient>> m_peerClients;
    std::mutex m_peerClientsMutex;

    std::queue<std::unique_ptr<PeerClient>> m_deletedPeerClients;
    std::mutex m_deletedPeerClientsMutex;

    IServerPacketHandler* m_handler;
};