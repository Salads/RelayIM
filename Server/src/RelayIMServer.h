#pragma once

#include <WinSock2.h>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>

class RelayIMServer
{
public:
    bool Initialize();
    bool Start();
    void Stop();

    bool IsInitialized() const;

    void ListenForClients();

    void ProcessClient(SOCKET clientSocket, uint32_t clientID);

private:
    bool m_isInitialized = false;

    addrinfo* m_listenSocketInfo = nullptr;
    SOCKET m_listenSocket = INVALID_SOCKET;

    std::thread m_listenThread;

    std::atomic_uint32_t m_nextClientID{ 0 };

    std::unordered_map<SOCKET, int> m_connectedClients;
    std::mutex m_clientsMutex;

    std::atomic_uint32_t m_nextRoomID{ 0 };
    std::unordered_map<uint32_t, std::vector<int>> m_chatRooms; // roomID -> list of clientIDs
    std::mutex m_chatRoomsMutex;

    std::unordered_map<uint32_t, std::thread> m_clientThreads; // clientID - > thread
    std::mutex m_clientThreadsMutex;
};