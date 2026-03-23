#pragma once

#include <string>

#include "NetworkTypes.h"
#include "ChatRoomInfo.h"
#include "ChatMessage.h"

/// <summary>
/// Implements the handling functions for different application-level packets.
/// </summary>
class IRelayIMClientPacketHandler
{
public:
    // Local Client registration result
    virtual void OnRegisterResponse(PacketResponseReason, PeerID, std::string) = 0;

    // Requested information of all existing chat rooms result
    virtual void OnListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>>) = 0;

    // Local Client join existing chat room result
    virtual void OnJoinRoomResponse(PacketResponseReason, RoomID, std::string) = 0;

    // Local Client create chat room result (also joins room)
    virtual void OnCreateRoomResponse(PacketResponseReason, RoomID, std::string) = 0;

    // Any Client has sent a message to a chat room
    virtual void OnRoomUpdate_NewMessage(RoomID, PeerID, std::string) = 0;

    // Local Client joined a existing chat room with messages/clients
    virtual void OnRoomUpdate_FullUpdate(RoomID, std::shared_ptr<std::vector<ChatMessage>>) = 0;

    // Remote Client has joined a chat room we're in.
    virtual void OnRoomUpdate_UserJoined(RoomID, PeerID, std::string) = 0;

    // Remote Client has left a chat room we're in.
    virtual void OnRoomUpdate_UserLeft(RoomID, PeerID) = 0;

};
