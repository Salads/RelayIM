#include "RelayIMClient.h"
#include "BinaryReader.h"

#include <iostream>

bool RelayIMClient::Start()
{
    if (!m_clientNetwork.Initialize())
    {
        return false;
    }

    m_clientNetwork.OnPacketReceived = [this](std::vector<uint8_t>* packet)
    {
        HandleServerPacket(packet);
    };

    return true;
}

void RelayIMClient::HandleServerPacket(std::vector<uint8_t>* serverPacket)
{
    BinaryReader reader(serverPacket);
    PacketHeader header; reader.ReadHeader(header);

    switch(header.m_packetType)
    {
        case PacketType_RoomUpdate_MSG:
        {
            uint32_t roomID = 0; reader.ReadUInt32(roomID);
            std::string message; reader.ReadString(message);
            std::cout << "Received RoomUpdate serverPacket. (Room ID: " << roomID << ", Message: '" << message << "'" << std::endl;
            break;
        }
        default:
        {
            std::cout << "Received unknown server packet type: " << (int)header.m_packetType << std::endl;
            break;
        }
    }
}