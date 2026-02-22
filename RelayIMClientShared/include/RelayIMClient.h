#pragma once

#include <unordered_map>
#include <vector>
#include <string>

#include "Types.h"
#include "ClientNetworkInterface.h"

class RelayIMClient
{
public:
    
    bool Start();
    void Shutdown();
    
    void JoinChatRoom(RoomID roomID);
    void CreateChatRoom(std::string roomName);

    void SendMessageToRoom(RoomID roomID, std::string message);

private:
    void HandleServerPacket(std::vector<uint8_t>* serverPacket);

private:

    ClientNetworkInterface m_clientNetwork;

    RoomID m_currentRoomID;
    std::vector<std::string> m_messages;

    // TODO(Salads): Should we use PeerID for this? It's internal to server...
    std::unordered_map<PeerID, std::string> m_roomUsers; // PeerID -> Username
};