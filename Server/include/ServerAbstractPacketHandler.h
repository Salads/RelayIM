#ifndef SERVERABSTRACTPACKETHANDLER_H
#define SERVERABSTRACTPACKETHANDLER_H

#include <memory>

#include "NetworkPacket.h"

class ServerAbstractPacketHandler
{
public:
    virtual void onNewClient(PeerID newPeerID) = 0;
    virtual void onClientDisconnected(PeerID peerID) = 0;
    virtual void onPacketReceived(PeerID peerID, std::unique_ptr<NetworkPacket> packet) = 0;
};

#endif // SERVERABSTRACTPACKETHANDLER_H
