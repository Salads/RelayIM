#include <iostream>
#include <WinSock2.h>

#include "ClientNetworkInterface.h"
#include "PacketWriter.h"
#include "NetworkTypes.h"

int main()
{
    ClientNetworkInterface clientNetwork;

    if (!clientNetwork.Initialize())
    {
        std::cerr << "Failed to initialize client peer." << std::endl;
        return 1;
    }

    std::cout << "Client peer initialized successfully." << std::endl;

    std::string testRoomName = "Hello Test Room";
    std::string testMessage = "We're doing it!";

    std::vector<uint8_t> testPacket;
    PacketWriter writer(testPacket);

    // Header
    writer.WriteUInt32(0xDEADBEEF); // PassCode
    writer.WriteUInt8(1); // Version
    writer.WriteUInt8(PacketType_JoinChatRoom);

    // Payload
    writer.WriteUInt32(0); // RoomID, 0 for new room.
    writer.WriteUInt8(1); // Create room if it doesn't exist.
    writer.WriteString(testRoomName);

    clientNetwork.Send(testPacket);

    system("pause");

    clientNetwork.Shutdown();
}