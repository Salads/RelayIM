#include "ChatRoom.h"

ChatRoom::ChatRoom(RoomID roomID, std::string roomName)
    : m_roomId(roomID), m_roomName(std::move(roomName))
{}

void ChatRoom::addClient(PeerID peerID)
{
    m_clients.insert(peerID);
}

void ChatRoom::removeClient(PeerID peerID)
{
    m_clients.erase(peerID);
}

void ChatRoom::addMessage(const ChatMessage& message)
{
    m_messages.push_back(message);
}

std::string ChatRoom::getRoomName()
{
    return m_roomName;
}

std::vector<PeerID> ChatRoom::getClients()
{
    std::vector<PeerID> result(m_clients.begin(), m_clients.end());
    return result;
}

const std::vector<ChatMessage>* ChatRoom::getMessages()
{
    return &m_messages;
}