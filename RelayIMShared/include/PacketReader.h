#ifndef PACKETREADER_H
#define PACKETREADER_H

#include <cstdint>
#include <vector>
#include <string>

#include "PacketHeader.h"
#include "NetworkPacket.h"
#include "RoomID.h"
#include "PeerID.h"
#include "PacketResponseReason.h"

class PacketReader
{
public:
    PacketReader(const NetworkPacket *data);

    bool readRoomID(RoomID& out);
    bool readPeerID(PeerID& out);

    bool readUInt8(uint8_t& outValue);
    bool readUInt16(uint16_t& outValue);
    bool readUInt32(uint32_t& outValue);
    bool readString(std::string &outString);

    bool readPacketResponseReason(PacketResponseReason& reason);

    bool readHeader(PacketHeader& outHeader);

private:
    const std::vector<uint8_t> *m_data;
    const uint8_t *m_cursor;
    const uint8_t *m_end;
};

#endif // PACKETREADER_H