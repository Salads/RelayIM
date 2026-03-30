#ifndef RELAYIMCLIENT_H
#define RELAYIMCLIENT_H

#include <vector>
#include <string>
#include <functional>

#include "PacketData.h"
#include "ClientNetworkInterface.h"
#include "ChatRoomInfo.h"
#include "IClientPacketHandler.h"
#include "IRelayIMClientPacketHandler.h"
#include "RoomID.h"
#include "PeerID.h"
#include "PacketHeader.h"
#include "PacketWriter.h"
#include "PacketReader.h"

class RelayIMClient : public IClientPacketHandler
{
public:
    RelayIMClient(IRelayIMClientPacketHandler* handler);
    bool Initialize();
    bool Connect();
    void Shutdown();
    
    void SendConnect(std::string desiredUsername);
    void SendRequestAllChatRooms();
    void SendJoinChatRoom(RoomID roomID);
    void SendCreateChatRoom(std::string roomName);
    void SendLeaveChatRoom(RoomID roomID);
    void SendMessageToRoom(RoomID roomID, std::string message);

private:
    void OnPacketReceived(std::unique_ptr<NetworkPacket> serverPacket) override;
    void OnServerDisconnected() override;

private:
    ClientNetworkInterface m_clientNetwork;
    IRelayIMClientPacketHandler* m_handler;
};

#endif // RELAYIMCLIENT_H