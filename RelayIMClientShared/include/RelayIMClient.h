#pragma once

#include <vector>
#include <string>
#include <functional>

#include "Types.h"
#include "ClientNetworkInterface.h"
#include "ClientChatRoom.h"
#include "ChatRoomInfo.h"
#include "ClientUser.h"
#include "IClientPacketHandler.h"

class RelayIMClient : public IClientPacketHandler
{
public:
    RelayIMClient();
    bool Initialize();
    bool Connect();
    void Shutdown();
    
    void SendConnect(std::string desiredUsername);
    void SendRequestAllChatRooms();
    void SendJoinChatRoom(RoomID roomID);
    void SendCreateChatRoom(std::string roomName);
    void SendLeaveChatRoom(RoomID roomID);
    void SendMessageToRoom(RoomID roomID, std::string message);
    
    // Local Client registration result
    std::function<void(PacketResponseReason, PeerID, std::string)> OnRegisterResponse;

    // Requested information of all existing chat rooms result
    std::function<void(std::shared_ptr<std::vector<ChatRoomInfo>>)> OnListChatRoomsResponse;

    // Local Client join existing chat room result
    std::function<void(PacketResponseReason, RoomID, std::string)> OnJoinRoomResponse;

    // Local Client create chat room result (also joins room)
    std::function<void(PacketResponseReason, RoomID, std::string)> OnCreateRoomResponse;

    // Any Client has sent a message to a chat room
    std::function<void(RoomID, PeerID, std::string)> OnRoomUpdate_NewMessage;

    // Local Client joined a existing chat room with messages/clients
    std::function<void(RoomID, std::unique_ptr<std::vector<ChatMessage>>)> OnRoomUpdate_FullUpdate;

    // Remote Client has joined a chat room we're in.
    std::function<void(RoomID, PeerID, std::string)> OnRoomUpdate_UserJoined;

    // Remote Client has left a chat room we're in.
    std::function<void(RoomID, PeerID)> OnRoomUpdate_UserLeft;

private:
    void OnPacketReceived(std::unique_ptr<NetworkPacket> serverPacket) override;
    void OnServerDisconnected() override;

private:

    ClientNetworkInterface m_clientNetwork;
};