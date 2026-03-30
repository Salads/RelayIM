#define _WINSOCKAPI_ // Exclude Winsock from being included in windows.h
#include <windows.h> // For hooking the "close" button

#include <iostream>
#include <thread>
#include <chrono>
#include <conio.h>

#include "RelayIMServer.h"
#include "Logging.h"

RelayIMServer g_server;
std::atomic_bool g_running = true;

void Shutdown()
{
    std::cout << "Stopping server..." << std::endl;
    std::cout.flush();

    g_server.stop();

    std::cout << "Stopped server!" << std::endl;
    std::cout.flush();
}

BOOL WINAPI CtrlHandler(DWORD dwCtrlType) 
{
    if (dwCtrlType == CTRL_CLOSE_EVENT) 
    {
        g_running = false;
        return TRUE;
    }

    return FALSE;
}

void InputThreadFunc()
{
    while (g_running)
    {
        int keyPress = _getch();
        if (keyPress == 27) // ESCAPE in ASCII = 27
        {
            g_running = false;
            break;
        }
    }
}

int main()
{    
    Log::initialize("server.log");
    if (!SetConsoleCtrlHandler(CtrlHandler, true))
    {
        std::cerr << "Failed to set Console Window Control Handler" << std::endl;
        return 1;
    }

    std::thread inputESCThread(InputThreadFunc);

    if (!g_server.initializeServer())
    {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }

    g_server.start();

    while (g_running)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        g_server.update();
    };

    if (inputESCThread.joinable())
    {
        inputESCThread.join();
    }

    Shutdown();
    Log::destroy();
    
    return 0;
}