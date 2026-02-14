#include <WinSock2.h>
#include <iostream>
#include "Util.h"

void PrintWSAError(const char* message)
{
    char buffer[256];
    FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        WSAGetLastError(),
        0,
        buffer,
        sizeof(buffer),
        NULL
    );
    std::cerr << message << ": " << buffer << std::endl;
}