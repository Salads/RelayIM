#ifndef CLIENTNETWORKINTERFACE_H
#define CLIENTNETWORKINTERFACE_H

#include <iostream>
#include <vector>
#include <atomic>

#include "NetworkInterface.h"
#include "NetworkPacket.h"
#include "Util.h"
#include "ClientAbstractPacketHandler.h"
#include "PeerID.h"
#include "NetworkConfig.h"
#include "PacketType.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

class ClientNetworkInterface : public NetworkInterface
{
public:
    ClientNetworkInterface(ClientAbstractPacketHandler* handler);
    bool initializeInterface() override;
    bool connectToServer();
    void shutdownInterface() override;

    void receiveLoop();

    void sendPacket(PacketData &data);

private:

    addrinfo* m_addrInfo;

    SOCKET m_clientSocket = INVALID_SOCKET;
    std::atomic_bool m_running = false;

    std::vector<uint8_t> m_receiveBuffer;

    std::thread m_receiveThread;

    ClientAbstractPacketHandler* m_handler;
};

#endif // CLIENTNETWORKINTERFACE_H