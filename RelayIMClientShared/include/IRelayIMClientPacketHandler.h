#ifndef IRELAYIMCLIENTPACKETHANDLER_H
#define IRELAYIMCLIENTPACKETHANDLER_H

#include <string>
#include <memory>

#include "ChatRoomInfo.h"
#include "ChatMessage.h"
#include "PacketResponseReason.h"

/// <summary>
/// Implements the handling functions for different application-level packets.
/// </summary>
class IRelayIMClientPacketHandler
{
public:
    // Local Client registration result
    virtual void OnRegisterResponse(PacketResponseReason reason, PeerID peerID, std::string username) = 0;

    // Requested information of all existing chat rooms result
    virtual void OnListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>> chatrooms) = 0;

    // Local Client join existing chat room result
    virtual void OnJoinRoomResponse(PacketResponseReason reason, RoomID roomID, std::string roomname) = 0;

    // Local Client create chat room result (also joins room)
    virtual void OnCreateRoomResponse(PacketResponseReason reason, RoomID roomID, std::string roomname) = 0;

    // Any Client has sent a message to a chat room
    virtual void OnRoomUpdate_NewMessage(RoomID roomID, PeerID peerID, std::string message) = 0;

    // Local Client joined a existing chat room with messages/clients
    virtual void OnRoomUpdate_FullUpdate(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages) = 0;

    // Remote Client has joined a chat room we're in.
    virtual void OnRoomUpdate_UserJoined(RoomID roomID, PeerID peerID, std::string username) = 0;

    // Remote Client has left a chat room we're in.
    virtual void OnRoomUpdate_UserLeft(RoomID roomID, PeerID peerID) = 0;

};

#endif // IRELAYIMCLIENTPACKETHANDLER_H
