#pragma once

#include <vector>
#include <unordered_set>
#include "NetworkTypes.h"
#include "ChatMessage.h"

struct ClientChatRoom
{
    ClientChatRoom(RoomID roomID, std::string roomname);

    RoomID m_roomID;
    std::string m_roomname;

    std::vector<ChatMessage> m_messages;
    std::unordered_set<PeerID> m_users;
};