#pragma once

#include <WinSock2.h>
#include <vector>
#include <atomic>
#include <functional>
#include <mutex>

#include "NetworkInterface.h"

class ClientNetworkInterface : public NetworkInterface
{
public:
    bool Initialize() override;
    void Shutdown() override;

    void ReceiveLoop();

    void Send(std::vector<uint8_t> &data);

    std::function<void()> OnServerDisconnected;
    std::function<void(std::vector<uint8_t>*)> OnPacketReceived;

private:
    SOCKET m_clientSocket = INVALID_SOCKET;
    std::atomic_bool m_running = false;

    std::vector<uint8_t> m_receiveBuffer;
    std::mutex m_receiveBufferMutex;
};