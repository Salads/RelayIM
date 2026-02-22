#include <iostream>
#include <WinSock2.h>

#include "ClientNetworkInterface.h"
#include "BinaryWriter.h"
#include "BinaryReader.h"
#include "NetworkTypes.h"
#include "Types.h"
#include "PacketType.h"

void HandlePacket(std::vector<uint8_t>* serverPacket)
{
    BinaryReader reader(serverPacket);
    PacketHeader header; reader.ReadHeader(header);

    std::cout << "Received serverPacket - PassCode: " << std::hex << header.m_passCode << ", Version: " << (int)header.m_version << ", PacketType: " << PacketTypeToString(header.m_packetType) << std::endl;
    // Handle different packet types here based on packetType.

    switch (header.m_packetType) 
    {
        case PacketType_RoomUpdate_MSG:
        {
            RoomID roomID = 0;   reader.ReadUInt32(roomID);
            std::string message; reader.ReadString(message);
            std::cout << "Received RoomUpdate serverPacket. (Room ID: " << roomID << ", Message: '" << message << "'" << std::endl;
        }
    }
}

int main()
{
    ClientNetworkInterface clientNetwork;

    if (!clientNetwork.Initialize())
    {
        std::cerr << "Failed to initialize client peer." << std::endl;
        return 1;
    }

    clientNetwork.OnPacketReceived = HandlePacket;

    std::thread receiveThread(&ClientNetworkInterface::ReceiveLoop, &clientNetwork);

    std::cout << "Client peer initialized successfully." << std::endl;

    std::string testRoomName = "Hello Test Room";
    std::string testMessage = "We're doing it!";

    std::vector<uint8_t> testPacket;
    BinaryWriter writer(testPacket);

    // Header
    writer.WriteUInt32(0xDEADBEEF); // PassCode
    writer.WriteUInt8(1); // Version
    writer.WriteUInt8(PacketType_JoinChatRoom);

    // Payload
    writer.WriteUInt32(0); // RoomID, 0 for new room.
    writer.WriteUInt8(1); // Create room if it doesn't exist.
    writer.WriteString(testRoomName);

    clientNetwork.Send(testPacket);

    while (true) {}

    clientNetwork.Shutdown();
}