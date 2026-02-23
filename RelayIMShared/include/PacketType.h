#pragma once

#include "Types.h"

const char* PacketTypeToString(uint8_t type);

#define PACKET_TYPES \
    X(PacketType_Connect)               \
    X(PacketType_JoinChatRoom)          \
    X(PacketType_CreateChatRoom)        \
    X(PacketType_LeaveChatRoom)         \
    X(PacketType_SendMessage)           \
    X(PacketType_RoomUpdate_MSG)        \
    X(PacketType_RoomUpdate_MSG_FULL)   \
    X(PacketType_RoomUpdate_UserJoined) \
    X(PacketType_RoomUpdate_UserLeft)   \
    X(PacketType_Response)

enum PacketType : uint8_t
{
#define X(name) name,
    PACKET_TYPES
#undef X
};

/*
    Packet Binary Format
    =======================

    HEADER:
        PacketSize (2 ubytes): Includes the size itself (for transmission purposes)
        PassCode   (4 ubytes): Help fight against malformed packets and offpath attacks. Should be a random value that is unlikely to appear in normal data.
                               Not useful for open source, and even a simplistic implementation, but better than nothing.
        Version #  (1 ubyte) : For version compatability checks
        PacketType (1 ubyte) : Determines the format of the payload

    PAYLOAD:
        PacketType_Connect:
            Username (VarLen) : Desired Username
        PacketType_JoinChatRoom:
            RoomID (4 ubytes) : The ID of the chat room to join.
        PacketType_CreateChatRoom:
            Room Name (VarLen): Room name to create.
        PacketType_LeaveChatRoom:
            RoomID (4 ubytes) : The ID of the chat room to join.
        PacketType_SendMessage:
            RoomID (4 ubytes) : The ID of the chat room to send the message to.
            Message (VarLen)  : The message to send. ASCII for now.
        PacketType_RoomUpdate_MSG :
            RoomID (4 ubytes) : The ID of the chat room that the update is for.
            Message (VarLen)  : The message that was sent to the chat room. ASCII for now.
            PeerID (4 ubytes) : Who sent the message (This is last, so we can write it last and minimize rewrite of data)
        PacketType_RoomUpdate_MSG_FULL :
            RoomID (4 ubytes) : The ID of the chat room that the update is for.
            ARRAY Message (VarLen)  : History of all messages in chat room.
        PacketType_RoomUpdate_UserJoined:
            RoomID (4 ubytes) : The ID of the chat room that the update is for.
            PeerID (4 ubytes) : The ID of the user that joined.
            Message (VarLen)  : Username of the new user.
        PacketType_RoomUpdate_UserLeft:
            RoomID (4 ubytes) : The ID of the chat room that the update is for.
            PeerID (4 ubytes) : The ID of the user that left.
        PacketType_Response   :
            Success (1 uByte) : 0 for failure, 1 for success.
*/