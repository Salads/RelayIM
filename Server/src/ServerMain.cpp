#define _WINSOCKAPI_ // Exclude Winsock from being included in windows.h
#include <windows.h> // For hooking the "close" button

#include <iostream>
#include <thread>
#include <chrono>
#include "RelayIMServer.h"
#include <fstream>

RelayIMServer g_server;
std::atomic_bool g_running = true;

BOOL WINAPI CtrlHandler(DWORD dwCtrlType) 
{
    if (dwCtrlType == CTRL_CLOSE_EVENT) 
    {
        g_running = false;

        std::cout << "Stopping server..." << std::endl;
        std::cout.flush();

        g_server.Stop();

        std::cout << "Stopped server!" << std::endl;
        std::cout.flush();

        system("pause");

        return TRUE;
    }

    return FALSE;
}

int main()
{    
    if (!SetConsoleCtrlHandler(CtrlHandler, true))
    {
        std::cerr << "Failed to set Console Window Control Handler" << std::endl;
        return 1;
    }

    if (!g_server.Initialize())
    {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }
     
    g_server.Start();

    while (g_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    };
    
    return 0;
}