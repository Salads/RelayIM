#pragma once

#include <WinSock2.h>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>

#include "Types.h"
#include "ChatRoom.h"
#include "ChatClient.h"

class RelayIMServer
{
public:
    bool Initialize();
    bool Start();
    void Stop();

    bool IsInitialized() const;

    void ListenForClients();

    void ProcessClient(ChatClient *client);

private:
    bool m_isInitialized = false;

    addrinfo* m_listenSocketInfo = nullptr;
    SOCKET m_listenSocket = INVALID_SOCKET;

    std::thread m_listenThread;

    std::atomic_uint32_t m_nextClientID{ 0 };
    std::atomic_uint32_t m_nextRoomID{ 0 };

    std::unordered_map<ClientID, std::unique_ptr<ChatClient>> m_clients;
    std::mutex m_clientsMutex;

    std::unordered_map<RoomID, ChatRoom> m_chatRooms;
    std::mutex m_chatRoomsMutex;
};