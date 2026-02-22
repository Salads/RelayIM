#pragma once

#include <WinSock2.h>
#include <cstdint>
#include <thread>
#include <vector>
#include <string>

#include "ChatClientStatus.h"
#include "NetworkTypes.h"

class ChatClient
{
public:
    ChatClient(uint32_t peerID)
        : m_peerID(peerID) {}
public:

    PeerID m_peerID = INVALID_PEER_ID;
    std::string m_username;
    std::vector<RoomID> m_chatRooms;

    ChatClientStatus m_status = ChatClientStatus_Fresh;
};