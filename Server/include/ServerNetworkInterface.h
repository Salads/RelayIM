#ifndef SERVERNETWORKINTERFACE_H
#define SERVERNETWORKINTERFACE_H

#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>
#include <memory>
#include <cstdint>
#include <queue>

#include "Util.h"
#include "NetworkInterface.h"
#include "PacketData.h"
#include "ChatClient.h"
#include "PeerClient.h"
#include "NetworkPacket.h"
#include "ServerAbstractPacketHandler.h"
#include "NetworkConfig.h"
#include "PacketType.h"
#include "PeerID.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

class ServerNetworkInterface : public NetworkInterface
{
public:
    ServerNetworkInterface(ServerAbstractPacketHandler* handler);

    bool initializeInterface() override;
    void shutdownInterface() override;

    void listenForClients();
    void receiveLoopForClient(PeerClient* client, SOCKET peerSocket);
    void sendLoopForClient(PeerClient* client, SOCKET peerSocket);

    void sendToClient(PeerID, PacketData* packet);
    void deleteDisconnectedClients();

private:
    void markPeerClientForDeletion(PeerID peerID);

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

    ServerAbstractPacketHandler* m_handler;
};

#endif // SERVERNETWORKINTERFACE_H