#ifndef CLIENTUSER_H
#define CLIENTUSER_H

#include <string>
#include <unordered_set>

#include "PeerID.h"
#include "RoomID.h"

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

#endif // CLIENTUSER_H