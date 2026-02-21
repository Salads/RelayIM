#include <iostream>
#include <WinSock2.h>

#include "ClientEndpoint.h"
#include "PacketWriter.h"
#include "NetworkTypes.h"

int main()
{
    ClientEndpoint clientPeer;

    if (!clientPeer.Initialize())
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
    writer.WriteUInt16(PACKETSIZE_HEADER + PACKETSIZE_JOINROOM_NOSTR + sizeof(uint16_t) + testRoomName.size());
    writer.WriteUInt32(0xDEADBEEF); // PassCode, should be random and unlikely to appear in normal data. Not useful for open source, but better than nothing.
    writer.WriteUInt8(1); // Version
    writer.WriteUInt8(PacketType_JoinChatRoom);

    // Payload
    writer.WriteUInt32(0); // RoomID, 0 for new room.
    writer.WriteUInt8(1); // Create room if it doesn't exist.
    writer.WriteString(testRoomName);

    clientPeer.Send(testPacket);

    system("pause");

    clientPeer.Shutdown();
}