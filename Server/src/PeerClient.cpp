#include "PeerClient.h"

void PeerClient::send(PacketData *packet)
{
    std::lock_guard<std::mutex> lock(m_sendBufferMutex);
    m_sendBuffer.insert(m_sendBuffer.end(), packet->begin(), packet->end());
    m_sendThreadCV.notify_one();
}

PeerClient::~PeerClient()
{
    shutdown(m_clientSocket, SD_BOTH);
    closesocket(m_clientSocket);

    if (m_sendThread.joinable())
    {
        markForDeletion(true);
        m_sendThreadCV.notify_one();
        m_sendThread.join();
    }

    if (m_receiveThread.joinable())
    {
        m_receiveThread.join();
    }
}

bool PeerClient::getMarkedForDeletion()
{
    return m_delete;
}

void PeerClient::markForDeletion(bool mark)
{
    clearSendBuffer();
    m_sendThreadCV.notify_one();
    m_delete = mark;
}

void PeerClient::clearSendBuffer()
{
    std::lock_guard lock(m_sendBufferMutex);
    m_sendBuffer.clear();
}