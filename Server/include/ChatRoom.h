#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include "ChatMessage.h"

class ChatRoom
{
public:
    int m_roomID;
    std::vector<ChatMessage> m_messages;
    std::unordered_set<PeerID> m_clientIDs;
};