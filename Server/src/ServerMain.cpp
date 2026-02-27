#include <iostream>
#include <thread>
#include <chrono>
#include "RelayIMServer.h"

int main()
{
    RelayIMServer server;

    if (!server.Initialize())
    {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }

    server.Start();

    while (true) 
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    };

    std::cout << "Stopping server..." << std::endl;
    server.Stop();
    
    return 0;
}