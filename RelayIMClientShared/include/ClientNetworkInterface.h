#pragma once

#include <WinSock2.h>
#include <vector>

#include "NetworkInterface.h"

class ClientNetworkInterface : public NetworkInterface
{
public:
    bool Initialize() override;
    void Shutdown() override;

    void Send(std::vector<uint8_t> &data);

private:
    SOCKET m_clientSocket = INVALID_SOCKET;
};