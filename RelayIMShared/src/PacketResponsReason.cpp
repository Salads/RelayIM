#include "PacketResponseReason.h"

const char* responseTypeToString(uint8_t type)
{
    switch(type)
    {
#define X(name) case PacketResponseReason::name: return #name;
        PACKET_RESPONSE_REASONS
#undef X
        default: return "Unknown";
    }
}