#ifndef CLIENTABSTRACTPACKETHANDLER_H
#define CLIENTABSTRACTPACKETHANDLER_H

#include <memory>

#include "NetworkPacket.h"

class ClientAbstractPacketHandler
{
public:

    /// <summary>
    /// This function will be ran on the ClientNetworkInterface receiving thread.
    /// </summary>
    /// <param name="packet"></param>
    virtual void onPacketReceived(std::unique_ptr<NetworkPacket> packet) = 0;
    virtual void onServerDisconnected() = 0;
};

#endif // CLIENTABSTRACTPACKETHANDLER_H