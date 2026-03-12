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
    Log::Initialize("simple-client-test.log");
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
        return 1;
    }

    client.SendConnect("Test Username");

    client.SendRequestAllChatRooms();

    client.SendCreateChatRoom("Hello Test Room");

    client.SendJoinChatRoom(0);

    client.SendMessageToRoom(0, "Test Message! Very Cool!");

    client.SendLeaveChatRoom(0);

    while (true) {}

    client.Shutdown();
    Log::Destroy();
}