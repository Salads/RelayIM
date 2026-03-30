#ifndef RELAYIMCLIENTABSTRACTPACKETHANDLER_H
#define RELAYIMCLIENTABSTRACTPACKETHANDLER_H

#include <string>
#include <memory>

#include "ChatRoomInfo.h"
#include "ChatMessage.h"
#include "PacketResponseReason.h"

/// <summary>
/// Implements the handling functions for different application-level packets.
/// </summary>
class RelayIMClientAbstractPacketHandler
{
public:
    // Local Client registration result
    virtual void onRegisterResponse(PacketResponseReason reason, PeerID peerID, std::string username) = 0;

    // Requested information of all existing chat rooms result
    virtual void onListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>> chatrooms) = 0;

    // Local Client join existing chat room result
    virtual void onJoinRoomResponse(PacketResponseReason reason, RoomID roomID, std::string roomname) = 0;

    // Local Client create chat room result (also joins room)
    virtual void onCreateRoomResponse(PacketResponseReason reason, RoomID roomID, std::string roomname) = 0;

    // Any Client has sent a message to a chat room
    virtual void onRoomUpdateNewMessage(RoomID roomID, PeerID peerID, std::string message) = 0;

    // Local Client joined a existing chat room with messages/clients
    virtual void onRoomUpdateFullUpdate(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages) = 0;

    // Remote Client has joined a chat room we're in.
    virtual void onRoomUpdateUserJoined(RoomID roomID, PeerID peerID, std::string username) = 0;

    // Remote Client has left a chat room we're in.
    virtual void onRoomUpdateUserLeft(RoomID roomID, PeerID peerID) = 0;

};

#endif // RELAYIMCLIENTABSTRACTPACKETHANDLER_H
