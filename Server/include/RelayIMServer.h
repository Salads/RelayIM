#pragma once

#include <WinSock2.h>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>

#include "Types.h"
#include "ChatRoom.h"
#include "ChatClient.h"
#include "ServerNetworkInterface.h"
#include "NetworkPacket.h"
#include "IServerPacketHandler.h"
#include "RoomID.h"
#include "PeerID.h"

class RelayIMServer : public IServerPacketHandler
{
public:
    RelayIMServer();
    bool Initialize();
    bool Start();
    void Stop();

    // Things that should happen on the main thread, go here.
    void Update();

    bool GetIsInitialized() const;
    bool GetIsRunning() const;
    void HandleNewClient(PeerID newPeerID);

private:
    void AddPacketToQueue(std::unique_ptr<NetworkPacket> newPacket);
    void ProcessClientPackets();

    bool IsUsernameTaken(std::string& newUsername);
    bool IsRoomnameTaken(std::string& newRoomname);

    void OnNewClient(PeerID newPeerID) override;
    void OnClientDisconnected(PeerID peerID) override;
    void OnPacketReceived(PeerID peerID, std::unique_ptr<NetworkPacket> packet) override;

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