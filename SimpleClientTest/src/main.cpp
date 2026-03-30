#include <iostream>
#include <WinSock2.h>

#include "ClientNetworkInterface.h"
#include "PacketWriter.h"
#include "PacketReader.h"
#include "Util.h"
#include "SimpleClientTestPacketHandler.h"

int main()
{
    Log::initialize("simple-client-test.log");
    SimpleClientTestPacketHandler handler;

    if (!handler.initialize())
    {
        std::cerr << "Failed to initialize client." << std::endl;
        return 1;
    }

    if (!handler.connectToServer())
    {
        std::cout << "Client failed to connect." << std::endl;
        return 1;
    }

    bool success = handler.testStandardSequence();
    std::cout << "Final Result: " << success << std::endl;
    while(true) {};

    Log::destroy();
}