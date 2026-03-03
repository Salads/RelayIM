#include "PacketType.h"
#include "Types.h"

const char* PacketTypeToString(uint8_t type)
{
    switch (type)
    {
#define X(name) case PacketType::name: return #name;
        PACKET_TYPES
#undef X
    default: return "Unknown";
    }
}

const char* ResponseTypeToString(uint8_t type)
{
    switch (type)
    {
#define X(name) case PacketResponseReason::name: return #name;
        PACKET_RESPONSE_REASONS
#undef X
    default: return "Unknown";
    }
}