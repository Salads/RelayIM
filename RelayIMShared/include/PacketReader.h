#ifndef PACKETREADER_H
#define PACKETREADER_H

#include "PacketHeader.h"
#include "NetworkPacket.h"
#include "RoomID.h"
#include "PeerID.h"
#include "PacketResponseReason.h"

#include <vector>
#include <string>

class PacketReader
{
public:
    PacketReader(const NetworkPacket *data);

    bool ReadRoomID(RoomID& out);
    bool ReadPeerID(PeerID& out);

    bool ReadUInt8(uint8_t& outValue);
    bool ReadUInt16(uint16_t& outValue);
    bool ReadUInt32(uint32_t& outValue);
    bool ReadString(std::string &outString);

    bool ReadPacketResponseReason(PacketResponseReason& reason);

    bool ReadHeader(PacketHeader& outHeader);

private:
    const std::vector<uint8_t> *m_data;
    const uint8_t *m_cursor;
    const uint8_t *m_end;
};

#endif // PACKETREADER_H