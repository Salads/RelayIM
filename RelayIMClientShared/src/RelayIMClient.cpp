#include "RelayIMClient.h"
#include "PacketReader.h"

#include <iostream>

bool RelayIMClient::Start()
{
    if (!m_clientNetwork.Initialize())
    {
        return false;
    }

    m_clientNetwork.OnPacketReceived = [this](std::unique_ptr<NetworkPacket> packet)
    {
        HandleServerPacket(std::move(packet));
    };

    return true;
}

void RelayIMClient::HandleServerPacket(std::unique_ptr<NetworkPacket> serverPacket)
{
    PacketReader reader(serverPacket.get());
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