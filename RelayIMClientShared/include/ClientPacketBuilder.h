#pragma once

#include <vector>
#include <string>
#include "NetworkTypes.h"

/*
    NOTE(Salads): Just a way to keep the code for building packets on the client in a single place.
*/
class ClientPacketBuilder
{
public:
    PacketData BuildConnectPacket(std::string userName);
    PacketData BuildListChatRoomsPacket();
    PacketData BuildJoinChatRoomPacket(RoomID roomID);
    PacketData BuildCreateChatRoomPacket(std::string roomName);
    PacketData BuildLeaveChatRoomPacket(RoomID roomID);
    PacketData BuildSendMessagePacket(RoomID roomID, std::string message);
};