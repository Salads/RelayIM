#define _WINSOCKAPI_ // Exclude Winsock from being included in windows.h
#include <windows.h> // For hooking the "close" button

#include <iostream>
#include <thread>
#include <chrono>
#include "RelayIMServer.h"


std::atomic_bool g_running = true;

BOOL WINAPI CtrlHandler(DWORD dwCtrlType) 
{
    if (dwCtrlType == CTRL_CLOSE_EVENT) 
    {
        g_running = false;
        return TRUE;
    }

    return FALSE;
}


int main()
{
    RelayIMServer server;
    SetConsoleCtrlHandler(CtrlHandler, true);

    if (!server.Initialize())
    {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }
     
    server.Start();

    while (g_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    };

    std::cout << "Stopping server..." << std::endl;
    server.Stop();

    system("pause");
    
    return 0;
}