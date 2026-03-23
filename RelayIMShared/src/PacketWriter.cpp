#include "PacketWriter.h"

PacketWriter::PacketWriter(PacketData& buffer)
    : m_buffer(&buffer)
{}

void PacketWriter::WriteUInt8(uint8_t value)
{
    for (int i = 0; i < sizeof(uint8_t); i++)
    {
        m_buffer->push_back((value >> (i * 8)) & 0xFF);
    }
}

void PacketWriter::WriteUInt16(uint16_t value)
{
    for(int i = 0; i < sizeof(uint16_t); i++)
    {
        m_buffer->push_back((value >> (i * 8)) & 0xFF);
    }
}

void PacketWriter::WriteUInt32(uint32_t value)
{
    for (int i = 0; i < sizeof(uint32_t); i++)
    {
        m_buffer->push_back((value >> (i * 8)) & 0xFF);
    }
}

void PacketWriter::WriteString(const std::string& string)
{
    WriteUInt16(static_cast<uint16_t>(string.size()));
    m_buffer->insert(m_buffer->end(), string.begin(), string.end());
}

void PacketWriter::WriteString(const char* str)
{
    std::string strWrapper(str);
    WriteString(strWrapper);
}

void PacketWriter::Finalize()
{
    *reinterpret_cast<uint16_t*>(m_buffer->data()) = static_cast<uint16_t>(m_buffer->size());
}

void PacketWriter::RewindBytes(uint32_t numBytes)
{
    for (uint32_t i = 0; i < numBytes; i++)
    {
        m_buffer->pop_back();
    }
}

void PacketWriter::WriteHeader(PacketType packetType)
{
    WriteUInt16(0); // Temporary packet size
    WriteUInt32(NETWORK_PASSCODE);
    WriteUInt8(NETWORK_VERSION);
    WriteUInt8(packetType);
}
