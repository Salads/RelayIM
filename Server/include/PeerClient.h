#ifndef PEERCLIENT_H
#define PEERCLIENT_H

#include <cstdint>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "PacketData.h"
#include "PeerID.h"

#include <WinSock2.h>

class PeerClient
{
public:
    PeerClient(PeerID clientID, SOCKET clientSocket)
        : m_peerId(clientID), m_clientSocket(clientSocket) {}

    ~PeerClient();

    void send(PacketData *packet);
    bool getMarkedForDeletion();
    void markForDeletion(bool mark);

private:
    void clearSendBuffer();

public:
    std::atomic_bool m_delete = false;

    PeerID m_peerId;
    SOCKET m_clientSocket;
    std::thread m_receiveThread;
    std::vector<uint8_t> m_receiveBuffer;

    std::thread m_sendThread;
    std::condition_variable m_sendThreadCV;

    std::vector<uint8_t> m_sendBuffer;
    std::mutex m_sendBufferMutex;
};

#endif // PEERCLIENT_H