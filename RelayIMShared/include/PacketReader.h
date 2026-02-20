#pragma once

#include <vector>
#include <string>

#include "NetworkTypes.h"

class PacketReader
{
public:
    PacketReader(const std::vector<uint8_t> *data);

    bool ReadUInt8(uint8_t& outValue);
    bool ReadUInt16(uint16_t& outValue);
    bool ReadUInt32(uint32_t& outValue);
    bool ReadString(std::string &outString);

private:
    const std::vector<uint8_t> *m_data;
    const uint8_t *m_cursor;
    const uint8_t *m_end;
};