#ifndef PACKETRESPONSEREASON_H
#define PACKETRESPONSEREASON_H

#include <cstdint>

const char* ResponseTypeToString(uint8_t type);

#define PACKET_RESPONSE_REASONS \
    X(Success)               \
    X(UsernameTaken)         \
    X(UsernameInvalid)       \
    X(ChatRoomDoesntExist)   \
    X(ChatRoomNameTaken)     \
    X(ChatRoomNameInvalid)   \
    X(Unspecified)               

enum PacketResponseReason : uint8_t
{
#define X(name) name,
    PACKET_RESPONSE_REASONS
#undef X
};

#endif // PACKETRESPONSEREASON_H