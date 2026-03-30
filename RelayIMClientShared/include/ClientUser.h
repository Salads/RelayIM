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

    std::string getUsername();
    PeerID getPeerId();
    size_t getNumRooms();

    void addToRoom(RoomID roomID);
    void removeFromRoom(RoomID roomID);

private:

    PeerID m_peerId;
    std::string m_username;

    std::unordered_set<RoomID> m_rooms;
};

#endif // CLIENTUSER_H