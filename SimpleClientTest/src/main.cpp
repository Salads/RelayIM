#include <iostream>
#include <WinSock2.h>

#include "ClientNetworkInterface.h"
#include "PacketWriter.h"
#include "PacketReader.h"
#include "NetworkTypes.h"
#include "Types.h"
#include "PacketType.h"
#include "ClientPacketBuilder.h"
#include "Util.h"
#include "SimpleClientTestPacketHandler.h"

int main()
{
    Log::Initialize("simple-client-test.log");
    SimpleClientTestPacketHandler handler;

    if (!handler.Initialize())
    {
        std::cerr << "Failed to initialize client." << std::endl;
        return 1;
    }

    if (!handler.Connect())
    {
        std::cout << "Client failed to connect." << std::endl;
        return 1;
    }

    bool success = handler.TestStandardSequence();
    std::cout << "Final Result: " << success << std::endl;
    while(true) {};

    Log::Destroy();
}