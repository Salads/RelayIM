#pragma once

#include <WinSock2.h>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <atomic>
#include <mutex>

#include "Types.h"
#include "ChatRoom.h"
#include "ChatClient.h"
#include "ServerNetworkInterface.h"

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

    bool IsUsernameTaken(std::string &newUsername);
    bool IsRoomnameTaken(std::string& newRoomname);

    void SendSimpleResponsePacket(PeerID peerID, bool success);

private:
    bool m_isInitialized = false;

    ServerNetworkInterface m_serverNetwork;

    std::atomic_uint32_t m_nextRoomID{ 0 };

    std::unordered_map<PeerID, std::unique_ptr<ChatClient>> m_clients;
    std::mutex m_clientsMutex;

    std::unordered_set<std::string> m_usedChatRoomNames;

    std::unordered_map<RoomID, std::unique_ptr<ChatRoom>> m_chatRooms;
    std::mutex m_chatRoomsMutex;
};