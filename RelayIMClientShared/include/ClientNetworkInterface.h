#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <atomic>
#include <functional>
#include <mutex>

#include "NetworkInterface.h"
#include "NetworkPacket.h"
#include "Util.h"
#include "IClientPacketHandler.h"
#include "PeerID.h"

class ClientNetworkInterface : public NetworkInterface
{
public:
    ClientNetworkInterface(IClientPacketHandler* handler);
    bool Initialize() override;
    bool Connect();
    void Shutdown() override;

    void ReceiveLoop();

    void Send(PacketData &data);

private:

    addrinfo* m_addrInfo;

    SOCKET m_clientSocket = INVALID_SOCKET;
    std::atomic_bool m_running = false;

    std::vector<uint8_t> m_receiveBuffer;

    std::thread m_receiveThread;

    IClientPacketHandler* m_handler;
};