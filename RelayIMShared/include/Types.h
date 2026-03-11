#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#define INVALID_PEER_ID -1
#define INVALID_ROOM_ID -1

typedef uint32_t PeerID;
typedef uint32_t RoomID;
typedef std::vector<uint8_t> PacketData;