#include "BinaryReader.h"

BinaryReader::BinaryReader(const NetworkPacket* data)
    : m_data(data->m_data.get()), m_cursor(m_data->data()), m_end(m_data->data() + m_data->size())
{}

bool BinaryReader::ReadUInt8(uint8_t& outValue)
{
    if (m_cursor + sizeof(uint8_t) > m_end)
    {
        return false;
    }

    outValue = *reinterpret_cast<const uint8_t*>(m_cursor);
    m_cursor += sizeof(uint8_t);
    return true;
}

bool BinaryReader::ReadUInt16(uint16_t& outValue)
{
    if (m_cursor + sizeof(uint16_t) > m_end)
    {
        return false;
    }

    outValue = *reinterpret_cast<const uint16_t*>(m_cursor);
    m_cursor += sizeof(uint16_t);
    return true;
}

bool BinaryReader::ReadUInt32(uint32_t& outValue)
{
    if (m_cursor + sizeof(uint32_t) > m_end)
    {
        return false;
    }

    outValue = *reinterpret_cast<const uint32_t*>(m_cursor);
    m_cursor += sizeof(uint32_t);
    return true;
}

bool BinaryReader::ReadString(std::string& outString)
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

bool BinaryReader::ReadHeader(PacketHeader& outHeader)
{
    if (m_cursor + sizeof(PacketHeader) > m_end)
    {
        return false;
    }

    outHeader = *reinterpret_cast<const PacketHeader*>(m_cursor);
    m_cursor += sizeof(PacketHeader);

    return true;
}