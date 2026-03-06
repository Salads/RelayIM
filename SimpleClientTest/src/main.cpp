#include <iostream>
#include <WinSock2.h>

#include "ClientNetworkInterface.h"
#include "BinaryWriter.h"
#include "PacketReader.h"
#include "NetworkTypes.h"
#include "Types.h"
#include "PacketType.h"
#include "ClientPacketBuilder.h"
#include "Util.h"
#include "RelayIMClient.h"

int main()
{
    RelayIMClient client;

    if (!client.Initialize())
    {
        std::cerr << "Failed to initialize client." << std::endl;
        return 1;
    }

    // TODO(Salads): Hook all client network interface callbacks

    if (!client.Connect())
    {
        std::cout << "Client failed to connect." << std::endl;
    }

    std::string testRoomName = "Hello Test Room";
    std::string testMessage = "We're doing it!";

    client.SendConnect("Test Username");
    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Sent %s\n", PacketTypeToString(PacketType_Connect));

    client.SendRequestAllChatRooms();
    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Sent %s\n", PacketTypeToString(PacketType_ListChatRooms));

    client.SendCreateChatRoom("Hello Test Room");
    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Sent %s\n", PacketTypeToString(PacketType_CreateChatRoom));

    client.SendJoinChatRoom(0);
    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Sent %s\n", PacketTypeToString(PacketType_JoinChatRoom));

    client.SendMessageToRoom(0, "Test Message! Very Cool!");
    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Sent %s\n", PacketTypeToString(PacketType_SendMessage));

    client.SendLeaveChatRoom(0);
    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Sent %s\n", PacketTypeToString(PacketType_LeaveChatRoom));

    while (true) {}

    client.Shutdown();
}