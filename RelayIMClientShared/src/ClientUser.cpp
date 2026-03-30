#include "ClientUser.h"

ClientUser::ClientUser(PeerID peerID, std::string username)
    :m_peerId(peerID), m_username(username)
{
}

std::string ClientUser::getUsername()
{
    return m_username;
}

PeerID ClientUser::getPeerId()
{
    return m_peerId;
}

void ClientUser::addToRoom(RoomID roomID)
{
    m_rooms.insert(roomID);
}

void ClientUser::removeFromRoom(RoomID roomID)
{
    m_rooms.erase(roomID);
}

size_t ClientUser::getNumRooms()
{
    return m_rooms.size();
}