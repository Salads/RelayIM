#pragma once

#include "ChatMessage.h"
#include "Types.h"
#include "RoomID.h"
#include "PeerID.h"

#include <string>
#include <vector>
#include <unordered_set>

class ChatRoom
{
public:
    ChatRoom(RoomID roomID, std::string roomName);

    void AddClient(PeerID peerID);
    void RemoveClient(PeerID peerID);
    void AddMessage(const ChatMessage& message);

    std::string GetRoomName();
    std::vector<PeerID> GetClients();
    const std::vector<ChatMessage>* GetMessages();

private:
    RoomID m_roomID;
    std::string m_roomName;
    std::vector<ChatMessage> m_messages;
    std::unordered_set<PeerID> m_clients;
};