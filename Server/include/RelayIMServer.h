#pragma once

#include <WinSock2.h>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>

#include "Types.h"
#include "ChatRoom.h"
#include "ChatClient.h"
#include "ServerEndpoint.h"

class RelayIMServer
{
public:
    bool Initialize();
    bool Start();
    void Stop();

    bool IsInitialized() const;
    void HandleNewClient(PeerID newPeerID);

private:
    void HandleClientPacket(PeerID peerID, std::vector<uint8_t>* packet);

private:
    bool m_isInitialized = false;

    ServerEndpoint m_serverEndpoint;

    std::atomic_uint32_t m_nextRoomID{ 0 };

    std::unordered_map<PeerID, std::unique_ptr<ChatClient>> m_clients;
    std::mutex m_clientsMutex;

    std::unordered_map<RoomID, ChatRoom> m_chatRooms;
    std::mutex m_chatRoomsMutex;
};