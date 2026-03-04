#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "Types.h"
#include "ClientNetworkInterface.h"
#include "ClientChatRoom.h"
#include "ChatRoomInfo.h"
#include "ClientUser.h"

class RelayIMClient
{
public:
    
    bool Start();
    void Shutdown();
    
    void SendConnect(std::string desiredUsername);
    void SendRequestAllChatRooms();
    void SendJoinChatRoom(RoomID roomID);
    void SendCreateChatRoom(std::string roomName);
    void SendLeaveChatRoom(RoomID roomID);
    void SendMessageToRoom(RoomID roomID, std::string message);

private:
    void HandleServerPacket(std::unique_ptr<NetworkPacket> serverPacket);

    void UpdateUsersForRoom(PeerID peerID, RoomID roomID, std::string username, bool remove = false);

private:

    ClientNetworkInterface m_clientNetwork;

    // Current client's peer id
    PeerID m_peerID = INVALID_PEER_ID;

    // Current client's username
    std::string m_username = "";

    // Chat rooms that client has joined
    std::unordered_map<RoomID, ClientChatRoom> m_joinedChatRooms;

    // Last snapshot of available chat rooms, given from server
    std::vector<ChatRoomInfo> m_chatRooms;

    // Map of PeerID -> ClientUser, which holds info about other users, like their username, and which rooms they're in (common to client)
    std::unordered_map<PeerID, ClientUser> m_users;
};