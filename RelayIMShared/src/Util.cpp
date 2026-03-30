#include "Util.h"

void printWSAError(const char* message)
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

std::string getLocalTimestamp()
{
    using namespace std::chrono;

    system_clock::time_point now = std::chrono::system_clock::now();
    zoned_time zoned = zoned_time(std::chrono::current_zone(), now);
    return std::format("{:%H-%M-%S}", zoned);
}