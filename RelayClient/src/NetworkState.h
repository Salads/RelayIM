#pragma once

#include "ChatRoomInfo.h"

class NetworkState
{
public:
    bool m_isConnected;
    
    ChatRoomInfo m_roomInfo;
    long long m_lastHeartbeatTime;
};