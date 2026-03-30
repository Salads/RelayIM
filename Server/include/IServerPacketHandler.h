#ifndef ISERVERPACKETHANDLER_H
#define ISERVERPACKETHANDLER_H

#include <memory>

#include "NetworkPacket.h"

class IServerPacketHandler
{
public:
    virtual void OnNewClient(PeerID newPeerID) = 0;
    virtual void OnClientDisconnected(PeerID peerID) = 0;
    virtual void OnPacketReceived(PeerID peerID, std::unique_ptr<NetworkPacket> packet) = 0;
};

#endif // ISERVERPACKETHANDLER_H
