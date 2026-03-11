#pragma once

#include <vector>
#include <string>

#include "NetworkTypes.h"
#include "PacketHeader.h"
#include "NetworkPacket.h"

class PacketReader
{
public:
    PacketReader(const NetworkPacket *data);

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