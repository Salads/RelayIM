#include "PeerClient.h"

#include "Logging.h"

void PeerClient::Send(PacketData *packet)
{
    std::lock_guard<std::mutex> lock(m_sendBufferMutex);
    m_sendBuffer.insert(m_sendBuffer.end(), packet->begin(), packet->end());
    m_sendThreadCV.notify_one();
}

PeerClient::~PeerClient()
{
    LogDepth(0, "Shutting down and closing peerclient sockets\n");
    shutdown(m_clientSocket, SD_BOTH);
    closesocket(m_clientSocket);

    LogDepth(0, "Joining Send Thread for client %u\n", m_peerID);
    if (m_sendThread.joinable())
    {
        MarkForDeletion(true);
        m_sendThreadCV.notify_one();
        m_sendThread.join();
    }

    LogDepth(0, "Joining Receive Thread for client %u\n", m_peerID);
    if (m_receiveThread.joinable())
    {
        m_receiveThread.join();
    }
}

bool PeerClient::GetMarkedForDeletion()
{
    return m_delete;
}

void PeerClient::MarkForDeletion(bool mark)
{
    ClearSendBuffer();
    m_sendThreadCV.notify_one();
    m_delete = mark;
}

void PeerClient::ClearSendBuffer()
{
    std::lock_guard lock(m_sendBufferMutex);
    m_sendBuffer.clear();
}