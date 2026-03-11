#pragma once

#include <string>

#include "Types.h"

struct ChatRoomInfo
{
    ChatRoomInfo();
    ChatRoomInfo(RoomID roomID, std::string roomname);

    RoomID m_roomID;
    std::string m_roomname;
};