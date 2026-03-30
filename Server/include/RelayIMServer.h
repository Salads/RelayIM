#ifndef RELAYIMSERVER_H
#define RELAYIMSERVER_H

#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

#include "PacketData.h"
#include "PacketReader.h"
#include "PacketWriter.h"
#include "ChatRoom.h"
#include "ChatClient.h"
#include "ServerNetworkInterface.h"
#include "NetworkPacket.h"
#include "ServerAbstractPacketHandler.h"
#include "RoomID.h"
#include "PeerID.h"

class RelayIMServer : public ServerAbstractPacketHandler
{
public:
    RelayIMServer();
    bool initializeServer();
    bool start();
    void stop();

    // Things that should happen on the main thread, go here.
    void update();

    bool isInitialized() const;
    bool getIsRunning() const;
    void handleNewClient(PeerID newPeerID);

private:
    void addPacketToQueue(std::unique_ptr<NetworkPacket> newPacket);
    void processClientPackets();

    bool isUsernameTaken(std::string& newUsername);
    bool isRoomnameTaken(std::string& newRoomname);

    void onNewClient(PeerID newPeerID) override;
    void onClientDisconnected(PeerID peerID) override;
    void onPacketReceived(PeerID peerID, std::unique_ptr<NetworkPacket> packet) override;

private:
    bool m_isInitialized = false;

    std::atomic_bool m_running = false;

    ServerNetworkInterface m_serverNetwork;

    std::atomic_uint32_t m_nextRoomID{ 0 };

    std::unordered_map<PeerID, std::unique_ptr<ChatClient>> m_clients;
    std::mutex m_clientsMutex;

    std::unordered_set<std::string> m_usedChatRoomNames;

    std::unordered_map<RoomID, std::unique_ptr<ChatRoom>> m_chatRooms;

    std::queue<std::unique_ptr<NetworkPacket>> m_incomingPackets;
    std::mutex m_incomingPacketsMutex;
    std::condition_variable m_incomingPacketsCV;

    std::thread m_packetHandlerThread;
};

#endif // RELAYIMSERVER_H