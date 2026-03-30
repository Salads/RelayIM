#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <vector>
#include <string>

#include "ChatClientStatus.h"
#include "PeerID.h"
#include "RoomID.h"

class ChatClient
{
public:
    ChatClient(PeerID peerID)
        : m_peerId(peerID) {}
public:

    PeerID m_peerId;
    std::string m_username;
    std::vector<RoomID> m_chatRooms;

    ChatClientStatus m_status = ChatClientStatus_Fresh;
};

#endif // CHATCLIENT_H