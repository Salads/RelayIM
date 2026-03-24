#include "ChatRoomInfo.h"

ChatRoomInfo::ChatRoomInfo()
    : m_roomname("")
{}

ChatRoomInfo::ChatRoomInfo(RoomID roomID, std::string roomname)
    : m_roomID(roomID), m_roomname(roomname)
{}
