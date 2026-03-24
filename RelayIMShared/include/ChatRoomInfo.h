#pragma once

#include <string>
#include "RoomID.h"

struct ChatRoomInfo
{
    ChatRoomInfo();
    ChatRoomInfo(RoomID roomID, std::string roomname);

    RoomID m_roomID;
    std::string m_roomname;
};