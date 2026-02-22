#pragma once

#include <vector>
#include <string>

#include "NetworkTypes.h"

class BinaryWriter
{
public:
    BinaryWriter(std::vector<uint8_t> &buffer);

    void WriteUInt8(uint8_t value);
    void WriteUInt16(uint16_t value);
    void WriteUInt32(uint32_t value);
    void WriteString(std::string &string);
    void WriteString(const char* str);

private:
    std::vector<uint8_t> *m_buffer;
};