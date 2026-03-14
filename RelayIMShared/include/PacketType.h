#pragma once

#include "Types.h"

const char* PacketTypeToString(uint8_t type);

#define PACKET_TYPES \
    X(PacketType_Connect)               \
    X(PacketType_ConnectResponse)       \
    X(PacketType_ListChatRooms)         \
    X(PacketType_ListChatRooms_Result)  \
    X(PacketType_JoinChatRoom)          \
    X(PacketType_JoinChatRoomResponse)  \
    X(PacketType_CreateChatRoom)        \
    X(PacketType_CreateChatRoomResponse)\
    X(PacketType_LeaveChatRoom)         \
    X(PacketType_SendMessage)           \
    X(PacketType_RoomUpdate_MSG)        \
    X(PacketType_RoomUpdate_FULL)       \
    X(PacketType_RoomUpdate_UserJoined) \
    X(PacketType_RoomUpdate_UserLeft)   

enum PacketType : uint8_t
{
#define X(name) name,
    PACKET_TYPES
#undef X
};

const char* ResponseTypeToString(uint8_t type);

#define PACKET_RESPONSE_REASONS \
    X(Success)               \
    X(UsernameTaken)         \
    X(UsernameInvalid)       \
    X(ChatRoomDoesntExist)   \
    X(ChatRoomNameTaken)     \
    X(ChatRoomNameInvalid)   \
    X(Unspecified)               

enum PacketResponseReason : uint8_t
{
#define X(name) name,
    PACKET_RESPONSE_REASONS
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
            Username:str - Username

        PacketType_ConnectResponse:
            Reason:1u    - Success/Error Reason
            PeerID:4u    - client's own peer id
            Username:str - client's requested username

        PacketType_ListChatRooms:
            nothing

        PacketType_ListChatRooms_Result:
            ARRAY
                RoomID:4u    - id of room
                Roomname:str - name of room

        PacketType_JoinChatRoom:
            RoomID:4u - room id to join

        PacketType_JoinChatRoomResponse:
            Reason:1u    - ErrorReason (Success means client gets room update packet)
            RoomID:4u    - Joined chat room id
            Roomname:str - join chat room name
            
        PacketType_CreateChatRoom:
            Room Name:str - name of room to create

        PacketType_CreateChatRoomResponse:
            Reason:1u     - Success/ErrorReason
            RoomID:4u     - room id of new chat room
            Room Name:str - room name of new chat room
            
        PacketType_LeaveChatRoom:
            RoomID:4u - id of room to leave
            
        PacketType_SendMessage:
            RoomID:4u   - id of room to send message to
            Message:str - message to send

        PacketType_RoomUpdate_MSG :
            RoomID:4u   - room id that message was sent to
            PeerID:4u   - user id that sent the message
            Message:str - message that user sent

        PacketType_RoomUpdate_FULL :
            RoomID:4u - room id this update is for
            ARRAY
                PeerID:4u    - users in the chat room
                Username:str - usernames in the chat room
            ARRAY
                PeerID:4u   - user id that sent the message
                Message:str - message that user sent

        PacketType_RoomUpdate_UserJoined:
            RoomID:4u    - room this update is for
            PeerID:4u    - id of user that joined the room
            Username:str - username of user that joined the room

        PacketType_RoomUpdate_UserLeft:
            RoomID:4u - room that user left
            PeerID:4u - user that left the room
*/