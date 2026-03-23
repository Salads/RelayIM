#pragma once

#include <memory>

#include "NetworkTypes.h"
#include "NetworkPacket.h"

class IClientPacketHandler
{
public:

    /// <summary>
    /// This function will be ran on the ClientNetworkInterface receiving thread.
    /// </summary>
    /// <param name="packet"></param>
    virtual void OnPacketReceived(std::unique_ptr<NetworkPacket> packet) = 0;
    virtual void OnServerDisconnected() = 0;
};