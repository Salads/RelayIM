#ifndef NETWORKPACKET_H
#define NETWORKPACKET_H

#include <memory>

#include "PacketData.h"
#include "PeerID.h"

struct NetworkPacket
{
    NetworkPacket(PeerID _peerID, std::unique_ptr<PacketData> _packetData);

    PeerID m_peerID;
    std::unique_ptr<PacketData> m_data;
};

#endif // NETWORKPACKET_H