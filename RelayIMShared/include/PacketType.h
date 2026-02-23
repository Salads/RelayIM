#pragma once

#include "Types.h"

const char* PacketTypeToString(uint8_t type);

#define PACKET_TYPES \
    X(PacketType_Connect)               \
    X(PacketType_ListChatRooms)         \
    X(PacketType_ListChatRooms_Result)  \
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
        PacketType_ListChatRooms:
            NOTHING
        PacketType_ListChatRooms_Result: Sent as a response to PacketType_ListChatRooms
            ARRAY (VarLen)
                RoomID (4 ubytes)
                String (VarLen) : Room Name
        PacketType_JoinChatRoom:
            RoomID (4 ubytes) :
        PacketType_CreateChatRoom:
            Room Name (VarLen): Room name to create.
        PacketType_LeaveChatRoom:
            RoomID (4 ubytes) :
        PacketType_SendMessage:
            RoomID (4 ubytes) :
            Message (VarLen)  :
        PacketType_RoomUpdate_MSG :
            RoomID (4 ubytes) :
            Message (VarLen)  :
            PeerID (4 ubytes) : Sender
        PacketType_RoomUpdate_MSG_FULL :
            RoomID (4 ubytes) :
            ARRAY Message (VarLen):
        PacketType_RoomUpdate_UserJoined:
            RoomID (4 ubytes) :
            PeerID (4 ubytes) :
            Username (VarLen) :
        PacketType_RoomUpdate_UserLeft:
            RoomID (4 ubytes) :
            PeerID (4 ubytes) :
        PacketType_Response   :
            Success (1 uByte) : 0 for failure, 1 for success.
*/