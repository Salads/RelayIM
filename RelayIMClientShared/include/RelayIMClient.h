#ifndef RELAYIMCLIENT_H
#define RELAYIMCLIENT_H

#include <vector>
#include <string>
#include <functional>

#include "PacketData.h"
#include "ClientNetworkInterface.h"
#include "ChatRoomInfo.h"
#include "ClientAbstractPacketHandler.h"
#include "RelayIMClientAbstractPacketHandler.h"
#include "RoomID.h"
#include "PeerID.h"
#include "PacketHeader.h"
#include "PacketWriter.h"
#include "PacketReader.h"

class RelayIMClient : public ClientAbstractPacketHandler
{
public:
    RelayIMClient(RelayIMClientAbstractPacketHandler* handler);
    bool initialize();
    bool connectToServer();
    void shutdownClient();
    
    void sendConnect(std::string desiredUsername);
    void sendRequestAllChatRooms();
    void sendJoinChatRoom(RoomID roomID);
    void sendCreateChatRoom(std::string roomName);
    void sendLeaveChatRoom(RoomID roomID);
    void sendMessageToRoom(RoomID roomID, std::string message);

private:
    void onPacketReceived(std::unique_ptr<NetworkPacket> serverPacket) override;
    void onServerDisconnected() override;

private:
    ClientNetworkInterface m_clientNetwork;
    RelayIMClientAbstractPacketHandler* m_handler;
};

#endif // RELAYIMCLIENT_H