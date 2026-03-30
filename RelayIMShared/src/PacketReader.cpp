#include "PacketReader.h"

PacketReader::PacketReader(const NetworkPacket* data)
    : m_data(data->m_data.get()), m_cursor(m_data->data()), m_end(m_data->data() + m_data->size())
{}

bool PacketReader::readRoomID(RoomID& out)
{
    uint32_t val = 0;
    if(!readUInt32(val))
    {
        return false;
    }
    else
    {
        out = val;
        return true;
    }
}

bool PacketReader::readPeerID(PeerID& out)
{
    uint32_t val = 0;
    if(!readUInt32(val))
    {
        return false;
    }
    else
    {
        out = val;
        return true;
    }
}

bool PacketReader::readUInt8(uint8_t& outValue)
{
    if (m_cursor + sizeof(uint8_t) > m_end)
    {
        return false;
    }

    outValue = *reinterpret_cast<const uint8_t*>(m_cursor);
    m_cursor += sizeof(uint8_t);
    return true;
}

bool PacketReader::readUInt16(uint16_t& outValue)
{
    if (m_cursor + sizeof(uint16_t) > m_end)
    {
        return false;
    }

    outValue = *reinterpret_cast<const uint16_t*>(m_cursor);
    m_cursor += sizeof(uint16_t);
    return true;
}

bool PacketReader::readUInt32(uint32_t& outValue)
{
    if (m_cursor + sizeof(uint32_t) > m_end)
    {
        return false;
    }

    outValue = *reinterpret_cast<const uint32_t*>(m_cursor);
    m_cursor += sizeof(uint32_t);
    return true;
}

bool PacketReader::readString(std::string& outString)
{
    uint16_t stringLength = 0;
    if (!readUInt16(stringLength))
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

bool PacketReader::readHeader(PacketHeader& outHeader)
{
    if (m_cursor + sizeof(PacketHeader) > m_end)
    {
        return false;
    }

    outHeader = *reinterpret_cast<const PacketHeader*>(m_cursor);
    m_cursor += sizeof(PacketHeader);

    return true;
}

bool PacketReader::readPacketResponseReason(PacketResponseReason& reason)
{
    uint8_t data = 0;
    bool result = readUInt8(data);
    reason = static_cast<PacketResponseReason>(data);
    return result;
}
