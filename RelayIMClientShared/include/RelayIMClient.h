#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "Types.h"
#include "ClientNetworkInterface.h"
#include "ClientChatRoom.h"

class RelayIMClient
{
public:
    
    bool Start();
    void Shutdown();
    
    void Connect(std::string desiredUsername);
    void JoinChatRoom(RoomID roomID);
    void CreateChatRoom(std::string roomName);
    void LeaveChatRoom(RoomID roomID);
    void SendMessageToRoom(RoomID roomID, std::string message);

private:
    void HandleServerPacket(std::vector<uint8_t>* serverPacket);

private:

    ClientNetworkInterface m_clientNetwork;

    std::unordered_map<RoomID, ClientChatRoom> m_chatRooms;

    // TODO(Salads): Should we use PeerID for this? It's internal to server...
    std::unordered_map<PeerID, std::string> m_usernames; // PeerID -> Username
};