#include "BinaryWriter.h"

BinaryWriter::BinaryWriter(std::vector<uint8_t>& buffer)
    : m_buffer(&buffer)
{
    WriteUInt16(0); // Placeholder for packet size, will be updated later.
}

void BinaryWriter::WriteUInt8(uint8_t value)
{
    for (int i = 0; i < sizeof(uint8_t); i++)
    {
        m_buffer->push_back((value >> (i * 8)) & 0xFF);
    }
}

void BinaryWriter::WriteUInt16(uint16_t value)
{
    for(int i = 0; i < sizeof(uint16_t); i++)
    {
        m_buffer->push_back((value >> (i * 8)) & 0xFF);
    }
}

void BinaryWriter::WriteUInt32(uint32_t value)
{
    for (int i = 0; i < sizeof(uint32_t); i++)
    {
        m_buffer->push_back((value >> (i * 8)) & 0xFF);
    }
}

void BinaryWriter::WriteString(std::string& string)
{
    WriteUInt16(static_cast<uint16_t>(string.size()));
    m_buffer->insert(m_buffer->end(), string.begin(), string.end());
}
