#ifndef PACKETWRITER_H
#define PACKETWRITER_H

#include "RoomID.h"
#include "PeerID.h"
#include "PacketType.h"
#include "PacketData.h"
#include "NetworkConfig.h"

#include <vector>
#include <string>


class PacketWriter
{
public:
    PacketWriter(PacketData &buffer);

    void WriteRoomID(const RoomID& roomID);
    void WritePeerID(const PeerID& peerID);

    void WriteUInt8(uint8_t value);
    void WriteUInt16(uint16_t value);
    void WriteUInt32(uint32_t value);
    void WriteString(const std::string &string);
    void WriteString(const char* str);

    void WriteHeader(PacketType packetType);

    void RewindBytes(uint32_t numBytes);

    // Updates the first two bytes to be the total size of the buffer.
    void Finalize();

private:
    std::vector<uint8_t> *m_buffer;
};

#endif // PACKETWRITER_H