#pragma once
#include <cstdint>

class NetworkPacker
{
public:
    NetworkPacker();
    ~NetworkPacker();

private:
    uint8_t m_buffer[1024] = { 0 };
};