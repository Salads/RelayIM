#pragma once

#include <WinSock2.h>
#include <cstdint>
#include <thread>
#include <vector>
#include <string>

#include "ChatClientStatus.h"
#include "NetworkTypes.h"
#include "PeerID.h"
#include "RoomID.h"

class ChatClient
{
public:
    ChatClient(PeerID peerID)
        : m_peerID(peerID) {}
public:

    PeerID m_peerID;
    std::string m_username;
    std::vector<RoomID> m_chatRooms;

    ChatClientStatus m_status = ChatClientStatus_Fresh;
};