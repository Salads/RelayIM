#pragma once

#include <WinSock2.h>
#include <thread>
#include <vector>
#include <condition_variable>

#include "Types.h"
#include "NetworkTypes.h"

struct PeerClient
{
    PeerClient(PeerID clientID, SOCKET clientSocket)
        : m_peerID(clientID), m_clientSocket(clientSocket) {}

    void Send(PacketData *packet);

    PeerID m_peerID = INVALID_PEER_ID;
    SOCKET m_clientSocket;
    std::thread m_receiveThread;
    std::vector<uint8_t> m_receiveBuffer;

    std::thread m_sendThread;
    std::condition_variable m_sendThreadCV;

    std::vector<uint8_t> m_sendBuffer;
    std::mutex m_sendBufferMutex;
};