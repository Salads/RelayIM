#pragma once

enum PacketType
{
    PacketType_Connect = 0,
    PacketType_Disconnect,
    PacketType_HeartBeat,
    PacketType_CreateChatRoom,
    PacketType_JoinChatRoom,
    PacketType_LeaveChatRoom,
    PacketType_SendMessage,
};