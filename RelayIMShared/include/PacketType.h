#pragma once

enum PacketType : uint8_t
{
    PacketType_JoinChatRoom, // Can both join existing and create new chat room.
    PacketType_LeaveChatRoom,
    PacketType_SendMessage,
    PacketType_RoomUpdate, // Server -> Client only. Sent when a new message is sent to a chat room
};

// convenience defines
#define PACKETSIZE_HEADER (sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint8_t))
#define PACKETSIZE_JOINROOM_NOSTR (sizeof(uint32_t) + sizeof(uint8_t)) // RoomID + Create (Not including variable length RoomName)
#define PACKETSIZE_LEAVEROOM (sizeof(uint32_t))
#define PACKETSIZE_SENDMESSAGE_NOSTR (sizeof(uint32_t)) // (Not including variable length Message) 
#define PACKETSIZE_ROOMUPDATE_NOSTR (sizeof(uint32_t))  // (Not including variable length Message)

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
        PacketType_JoinChatRoom:
            RoomID (4 ubytes): The ID of the chat room to join.
            Create (1 ubyte) : Whether to create the chat room if it doesn't exist.
            RoomName (VarLen): The name of the chat room to create. Only used if Create is true.
        PacketType_LeaveChatRoom:
            RoomID (4 ubytes): The ID of the chat room to join.
        PacketType_SendMessage:
            RoomID (4 ubytes): The ID of the chat room to send the message to.
            Message (VarLen) : The message to send. ASCII for now.
        PacketType_RoomUpdate:
            RoomID (4 ubytes): The ID of the chat room that the update is for.
            Message (VarLen) : The message that was sent to the chat room. ASCII for now.
*/