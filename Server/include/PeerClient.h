#pragma once

#include <WinSock2.h>
#include <thread>
#include <vector>
#include <condition_variable>

#include "Types.h"
#include "PeerID.h"

class PeerClient
{
public:
    PeerClient(PeerID clientID, SOCKET clientSocket)
        : m_peerID(clientID), m_clientSocket(clientSocket) {}

    ~PeerClient();

    void Send(PacketData *packet);
    bool GetMarkedForDeletion();
    void MarkForDeletion(bool mark);

private:
    void ClearSendBuffer();

public:
    std::atomic_bool m_delete = false;

    PeerID m_peerID;
    SOCKET m_clientSocket;
    std::thread m_receiveThread;
    std::vector<uint8_t> m_receiveBuffer;

    std::thread m_sendThread;
    std::condition_variable m_sendThreadCV;

    std::vector<uint8_t> m_sendBuffer;
    std::mutex m_sendBufferMutex;
};