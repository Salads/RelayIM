#pragma once

#include <vector>
#include <unordered_set>
#include "ChatMessage.h"
#include "RoomID.h"
#include "PeerID.h"

struct ClientChatRoom
{
    ClientChatRoom(RoomID roomID, std::string roomname);

    RoomID m_roomID;
    std::string m_roomname;

    std::vector<ChatMessage> m_messages;
    std::unordered_set<PeerID> m_users;
};