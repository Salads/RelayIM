#include "NetworkPacket.h"

NetworkPacket::NetworkPacket(PeerID _peerID, std::unique_ptr<PacketData> _packetData)
    :m_peerId(_peerID), m_data(std::move(_packetData))
{}
