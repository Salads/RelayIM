#include "PacketReader.h"

PacketReader::PacketReader(const std::vector<uint8_t>* data)
    : m_data(data), m_cursor(m_data->data()), m_end(m_data->data() + m_data->size())
{}

bool PacketReader::ReadUInt8(uint8_t& outValue)
{
    if (m_cursor + sizeof(uint8_t) > m_end)
    {
        return false;
    }

    outValue = *reinterpret_cast<const uint8_t*>(m_cursor);
    m_cursor += sizeof(uint8_t);
    return true;
}

bool PacketReader::ReadUInt16(uint16_t& outValue)
{
    if (m_cursor + sizeof(uint16_t) > m_end)
    {
        return false;
    }

    outValue = *reinterpret_cast<const uint16_t*>(m_cursor);
    m_cursor += sizeof(uint16_t);
    return true;
}

bool PacketReader::ReadUInt32(uint32_t& outValue)
{
    if (m_cursor + sizeof(uint32_t) > m_end)
    {
        return false;
    }

    outValue = *reinterpret_cast<const uint32_t*>(m_cursor);
    m_cursor += sizeof(uint32_t);
    return true;
}

bool PacketReader::ReadString(std::string& outString)
{
    uint16_t stringLength = 0;
    if (!ReadUInt16(stringLength))
    {
        return false;
    }

    if (m_cursor + stringLength > m_end)
    {
        return false;
    }

    outString = std::string(m_cursor, m_cursor + stringLength);
    m_cursor += stringLength;
    return true;
}