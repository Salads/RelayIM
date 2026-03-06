#pragma once

#include <WinSock2.h>
#include <vector>
#include <atomic>
#include <functional>
#include <mutex>

#include "NetworkInterface.h"
#include "NetworkPacket.h"

class ClientNetworkInterface : public NetworkInterface
{
public:
    bool Initialize() override;
    bool Connect();
    void Shutdown() override;

    void ReceiveLoop();

    void Send(PacketData &data);

    std::function<void()> OnServerDisconnected;
    std::function<void(std::unique_ptr<NetworkPacket>)> OnPacketReceived;

private:

    addrinfo* m_addrInfo;

    SOCKET m_clientSocket = INVALID_SOCKET;
    std::atomic_bool m_running = false;

    std::vector<uint8_t> m_receiveBuffer;

    std::thread m_receiveThread;
};