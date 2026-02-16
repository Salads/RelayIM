#pragma once

#include <WinSock2.h>
#include <thread>
#include "Types.h"

struct PeerClient
{
    PeerClient(PeerID clientID, SOCKET clientSocket)
        : m_peerID(clientID), m_clientSocket(clientSocket) {}

    PeerID m_peerID = INVALID_PEER_ID;
    SOCKET m_clientSocket;
    std::thread m_thread;
};