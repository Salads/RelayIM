#pragma once

#include <WinSock2.h>
#include <vector>

#include "NetworkEndpoint.h"

class ClientEndpoint : public NetworkEndpoint
{
public:
    bool Initialize() override;
    void Shutdown() override;

    void Send(std::vector<uint8_t> &data);

private:
    SOCKET m_clientSocket = INVALID_SOCKET;
};