#ifndef PACKETHEADER_H
#define PACKETHEADER_H

#include <cstdint>

#include "PacketType.h"

#pragma pack(push, 1)
struct PacketHeader
{
    uint16_t    m_size;
    uint32_t    m_passCode;
    uint8_t     m_version;
    PacketType  m_packetType;
};
#pragma pack(pop)

#endif // PACKETHEADER_H