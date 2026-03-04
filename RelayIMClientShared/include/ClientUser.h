#pragma once

#include <string>
#include <unordered_set>

#include "Types.h"

class ClientUser
{
public:
    ClientUser(PeerID peerID, std::string username);

    std::string GetUsername();
    PeerID GetPeerID();
    size_t GetNumRooms();

    void AddToRoom(RoomID roomID);
    void RemoveFromRoom(RoomID roomID);

private:

    PeerID m_peerID;
    std::string m_username;

    std::unordered_set<RoomID> m_rooms;
};