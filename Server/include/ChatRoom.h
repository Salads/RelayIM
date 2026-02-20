#pragma once

#include <string>
#include <vector>
#include <unordered_set>

#include "ChatMessage.h"
#include "Types.h"

class ChatRoom
{
public:
    ChatRoom(RoomID roomID);

    int m_roomID;
    std::vector<ChatMessage> m_messages;
    std::unordered_set<PeerID> m_clientIDs;
};