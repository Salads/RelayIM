#define _WINSOCKAPI_ // Exclude Winsock from being included in windows.h
#include <windows.h> // For hooking the "close" button

#include <iostream>
#include <thread>
#include <chrono>=
#include <conio.h>

#include "RelayIMServer.h"

RelayIMServer g_server;
std::atomic_bool g_running = true;

void Shutdown()
{
    std::cout << "Stopping server..." << std::endl;
    std::cout.flush();

    g_server.Stop();

    std::cout << "Stopped server!" << std::endl;
    std::cout.flush();
}

BOOL WINAPI CtrlHandler(DWORD dwCtrlType) 
{
    if (dwCtrlType == CTRL_CLOSE_EVENT) 
    {
        LogDepth(0, "Close button pressed! Quitting...\n");
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
            LogDepth(0, "ESC pressed! Quitting...\n");
            g_running = false;
            break;
        }
    }
}

int main()
{    
    if (!SetConsoleCtrlHandler(CtrlHandler, true))
    {
        std::cerr << "Failed to set Console Window Control Handler" << std::endl;
        return 1;
    }

    std::thread inputESCThread(InputThreadFunc);

    if (!g_server.Initialize())
    {
        std::cerr << "Failed to initialize server" << std::endl;
        return 1;
    }
     
    g_server.Start();

    while (g_running)
    {
        LogDepth(0, "g_server.Update()\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        g_server.Update();
    };

    LogDepth(0, "Joining input thread\n");
    if (inputESCThread.joinable())
    {
        inputESCThread.join();
    }

    Shutdown();
    
    return 0;
}