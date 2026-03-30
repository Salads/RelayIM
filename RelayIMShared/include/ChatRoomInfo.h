#ifndef CHATROOMINFO_H
#define CHATROOMINFO_H

#include <string>

#include "RoomID.h"

struct ChatRoomInfo
{
    ChatRoomInfo();
    ChatRoomInfo(RoomID roomID, std::string roomname);

    RoomID m_roomId;
    std::string m_roomname;
};

#endif // CHATROOMINFO_H