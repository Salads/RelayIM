#include <iostream>
#include <WinSock2.h>

#include "ClientPeer.h"

int main()
{
    ClientPeer clientPeer;

    if (!clientPeer.Initialize())
    {
        std::cerr << "Failed to initialize client peer." << std::endl;
        return 1;
    }

    std::cout << "Client peer initialized successfully." << std::endl;

    std::vector<uint8_t> testData = {'H', 'e', 'l', 'l', 'o'};
    clientPeer.Send(testData);

    system("pause");

    clientPeer.Shutdown();
}