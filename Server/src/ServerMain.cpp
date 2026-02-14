#include <iostream>
#include "RelayIMServer.h"

int main()
{
    RelayIMServer server;

    if (!server.Initialize())
    {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }

    // server.Run();

    std::cout << "Stopping server..." << std::endl;
    server.Stop();
    
    return 0;
}