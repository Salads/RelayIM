#ifndef PACKETWRITER_H
#define PACKETWRITER_H

#include <vector>
#include <string>

#include "RoomID.h"
#include "PeerID.h"
#include "PacketType.h"
#include "PacketData.h"
#include "NetworkConfig.h"

class PacketWriter
{
public:
    PacketWriter(PacketData &buffer);

    void writeRoomId(const RoomID& roomID);
    void writePeerId(const PeerID& peerID);

    void writeUInt8(uint8_t value);
    void writeUInt16(uint16_t value);
    void writeUInt32(uint32_t value);
    void writeString(const std::string &string);
    void writeString(const char* str);

    void writeHeader(PacketType packetType);

    void rewindBytes(uint32_t numBytes);

    // Updates the first two bytes to be the total size of the buffer.
    void finalize();

private:
    std::vector<uint8_t> *m_buffer;
};

#endif // PACKETWRITER_H