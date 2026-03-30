#ifndef CHATROOM_H
#define CHATROOM_H

#include <string>
#include <vector>
#include <unordered_set>

#include "ChatMessage.h"
#include "RoomID.h"
#include "PeerID.h"

class ChatRoom
{
public:
    ChatRoom(RoomID roomID, std::string roomName);

    void addClient(PeerID peerID);
    void removeClient(PeerID peerID);
    void addMessage(const ChatMessage& message);

    std::string getRoomName();
    std::vector<PeerID> getClients();
    const std::vector<ChatMessage>* getMessages();

private:
    RoomID m_roomId;
    std::string m_roomName;
    std::vector<ChatMessage> m_messages;
    std::unordered_set<PeerID> m_clients;
};

#endif // CHATROOM_H