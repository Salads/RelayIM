#include "ChatRoomInfo.h"

ChatRoomInfo::ChatRoomInfo()
    : m_roomID(INVALID_PEER_ID), m_roomname("")
{}

ChatRoomInfo::ChatRoomInfo(RoomID roomID, std::string roomname)
    : m_roomID(roomID), m_roomname(roomname)
{}
