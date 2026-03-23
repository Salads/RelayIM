#pragma once

#include <memory>

#include "NetworkTypes.h"
#include "NetworkPacket.h"

class IClientPacketHandler
{
public:
    virtual void OnPacketReceived(std::unique_ptr<NetworkPacket> packet) = 0;
    virtual void OnServerDisconnected() = 0;
};