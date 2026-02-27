#include "PeerClient.h"

void PeerClient::Send(PacketData *packet)
{
    std::lock_guard<std::mutex> lock(m_sendBufferMutex);
    m_sendBuffer.insert(m_sendBuffer.end(), packet->begin(), packet->end());
    m_sendThreadCV.notify_one();
}