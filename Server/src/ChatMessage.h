#pragma once

#include <cstdint>
#include <string>

class ChatMessage
{
public:
    uint32_t m_senderID;
    std::string m_messageText;
};