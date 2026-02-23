#pragma once

#include <vector>
#include <unordered_set>
#include "NetworkTypes.h"
#include "ChatMessage.h"

struct ClientChatRoom
{
    RoomID m_roomID;

    std::vector<ChatMessage> m_messages;
    std::unordered_set<PeerID> m_users;
};