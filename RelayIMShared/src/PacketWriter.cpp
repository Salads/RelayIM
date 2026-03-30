#include "PacketWriter.h"

PacketWriter::PacketWriter(PacketData& buffer)
    : m_buffer(&buffer)
{}

void PacketWriter::writeRoomId(const RoomID& roomID)
{
    writeUInt32(static_cast<uint32_t>(roomID));
}

void PacketWriter::writePeerId(const PeerID& peerID)
{
    writeUInt32(static_cast<uint32_t>(peerID));
}

void PacketWriter::writeUInt8(uint8_t value)
{
    for (int i = 0; i < sizeof(uint8_t); i++)
    {
        m_buffer->push_back((value >> (i * 8)) & 0xFF);
    }
}

void PacketWriter::writeUInt16(uint16_t value)
{
    for(int i = 0; i < sizeof(uint16_t); i++)
    {
        m_buffer->push_back((value >> (i * 8)) & 0xFF);
    }
}

void PacketWriter::writeUInt32(uint32_t value)
{
    for (int i = 0; i < sizeof(uint32_t); i++)
    {
        m_buffer->push_back((value >> (i * 8)) & 0xFF);
    }
}

void PacketWriter::writeString(const std::string& string)
{
    writeUInt16(static_cast<uint16_t>(string.size()));
    m_buffer->insert(m_buffer->end(), string.begin(), string.end());
}

void PacketWriter::writeString(const char* str)
{
    std::string strWrapper(str);
    writeString(strWrapper);
}

void PacketWriter::finalize()
{
    *reinterpret_cast<uint16_t*>(m_buffer->data()) = static_cast<uint16_t>(m_buffer->size());
}

void PacketWriter::rewindBytes(uint32_t numBytes)
{
    for (uint32_t i = 0; i < numBytes; i++)
    {
        m_buffer->pop_back();
    }
}

void PacketWriter::writeHeader(PacketType packetType)
{
    writeUInt16(0); // Temporary packet size
    writeUInt32(NETWORK_PASSCODE);
    writeUInt8(NETWORK_VERSION);
    writeUInt8(packetType);
}
