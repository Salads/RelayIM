#pragma once

#include "PacketData.h"
#include "PeerID.h"

#include <memory>

struct NetworkPacket
{
    NetworkPacket(PeerID _peerID, std::unique_ptr<PacketData> _packetData);

    PeerID m_peerID;
    std::unique_ptr<PacketData> m_data;
};