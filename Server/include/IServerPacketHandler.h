#pragma once

#include <memory>

#include "NetworkPacket.h"

class IServerPacketHandler
{
public:
    virtual void OnNewClient(PeerID newPeerID) = 0;
    virtual void OnClientDisconnected(PeerID peerID) = 0;
    virtual void OnPacketReceived(PeerID peerID, std::unique_ptr<NetworkPacket> packet) = 0;
};
