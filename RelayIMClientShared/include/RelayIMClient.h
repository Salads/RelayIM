#pragma once

#include <vector>
#include <string>
#include <functional>

#include "Types.h"
#include "ClientNetworkInterface.h"
#include "ClientChatRoom.h"
#include "ChatRoomInfo.h"
#include "ClientUser.h"

class RelayIMClient
{
public:
    
    bool Initialize();
    bool Connect();
    void Shutdown();
    
    void SendConnect(std::string desiredUsername);
    void SendRequestAllChatRooms();
    void SendJoinChatRoom(RoomID roomID);
    void SendCreateChatRoom(std::string roomName);
    void SendLeaveChatRoom(RoomID roomID);
    void SendMessageToRoom(RoomID roomID, std::string message);

    std::function<void(PeerID, std::string)> OnLocalRegistered;

    // Simple header information for every chat room. 
    std::function<void(std::shared_ptr<std::vector<ChatRoomInfo>>)> OnListChatRoomsReceived;

    // A new client's information. Could be through it joining a chat room we're in, or through us joining a chat room.
    std::function<void(PeerID, RoomID, std::string)> OnNewClientDiscovered;

    std::function<void(RoomID, std::string)> OnLocalChatRoomJoined;

    // A client has left a chat room.
    std::function<void(PeerID, RoomID)> OnClientLeftChatRoom;

    // When a single message has been received
    std::function<void(RoomID, PeerID, std::string)> OnMessageReceived;

    // All information for catching us up to the current state of the chat room.
    std::function<void(RoomID, std::unique_ptr<std::vector<ChatMessage>>)> OnRoomFullUpdate;

    std::function<void(PacketResponseReason)> OnResponseFailed;

private:
    void HandleServerPacket(std::unique_ptr<NetworkPacket> serverPacket);

private:

    ClientNetworkInterface m_clientNetwork;
};