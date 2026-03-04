#pragma once

#include "ClientUser.h"

ClientUser::ClientUser(PeerID peerID, std::string username)
    :m_peerID(peerID), m_username(username)
{
}

std::string ClientUser::GetUsername()
{
    return m_username;
}

PeerID ClientUser::GetPeerID()
{
    return m_peerID;
}

void ClientUser::AddToRoom(RoomID roomID)
{
    m_rooms.insert(roomID);
}

void ClientUser::RemoveFromRoom(RoomID roomID)
{
    m_rooms.erase(roomID);
}

size_t ClientUser::GetNumRooms()
{
    return m_rooms.size();
}