#include "ChatRoom.h"
#include "Types.h"

ChatRoom::ChatRoom(RoomID roomID, std::string roomName)
    : m_roomID(roomID), m_roomName(std::move(roomName))
{}

void ChatRoom::AddClient(PeerID peerID)
{
    m_clients.insert(peerID);
}

void ChatRoom::RemoveClient(PeerID peerID)
{
    m_clients.erase(peerID);
}

void ChatRoom::AddMessage(const ChatMessage& message)
{
    m_messages.push_back(message);
}

std::string ChatRoom::GetRoomName()
{
    return m_roomName;
}

std::vector<PeerID> ChatRoom::GetClients()
{
    std::vector<PeerID> result(m_clients.begin(), m_clients.end());
    return result;
}

const std::vector<ChatMessage>* ChatRoom::GetMessages()
{
    return &m_messages;
}