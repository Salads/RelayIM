#include <iostream>
#include <WinSock2.h>

#include "ClientNetworkInterface.h"
#include "BinaryWriter.h"
#include "PacketReader.h"
#include "NetworkTypes.h"
#include "Types.h"
#include "PacketType.h"
#include "ClientPacketBuilder.h"
#include "Util.h"

void HandlePacket(std::unique_ptr<NetworkPacket> serverPacket)
{
    std::unique_ptr<NetworkPacket> packet = std::move(serverPacket);
    PacketReader reader(packet.get());
    PacketHeader header; reader.ReadHeader(header);

    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Packet Received (%s): [%X, %u]\n", PacketTypeToString(header.m_packetType), header.m_passCode, header.m_version);

    switch (header.m_packetType) 
    {
        case PacketType_ConnectResponse:
        {
            uint8_t result; reader.ReadUInt8(result);
            if (result == PacketResponseReason::Success)
            {

            }
            else if (result == PacketResponseReason::UsernameTaken)
            {

            }

        } break;
        case PacketType_ListChatRooms_Result:
        {
            uint16_t nRooms = 0; reader.ReadUInt16(nRooms);
            for (int i = 0; i < nRooms; i++)
            {
                uint32_t roomID = 0; reader.ReadUInt32(roomID);
                std::string roomName; reader.ReadString(roomName);
                LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "Room %u: %s\n", roomID, roomName);
            }

            if (!nRooms)
            {
                LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "No chat rooms exist yet!\n");
            }

        } break;
        case PacketType_RoomUpdate_MSG:
        {
            RoomID roomID = 0;   reader.ReadUInt32(roomID);
            std::string message; reader.ReadString(message);
            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "New Message (Room %u): '%s'\n", roomID, message);
        } break;
        case PacketType_RoomUpdate_FULL:
        {
            RoomID roomID = 0;   reader.ReadUInt32(roomID);
            uint16_t nMessages = 0; reader.ReadUInt16(nMessages);
            for (int i = 0; i < nMessages; i++)
            {
                std::string message; reader.ReadString(message);
                LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "Message %d: '%s'\n", i, message);
            }
        } break;
        case PacketType_RoomUpdate_UserJoined:
        {
            RoomID roomID = 0; reader.ReadUInt32(roomID);
            PeerID peerID = 0; reader.ReadUInt32(peerID);
            std::string userName; reader.ReadString(userName);
            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "User '%s' (id %u) joined room %u\n", userName, peerID, roomID);
        } break;
        case PacketType_RoomUpdate_UserLeft:
        {
            RoomID roomID = 0; reader.ReadUInt32(roomID);
            PeerID peerID = 0; reader.ReadUInt32(peerID);
            LogDepthConditional(LOG_NETWORK_PACKETS_DATA, 1, "User id %u left room %u\n", peerID, roomID);
        } break;
        default:
        {

        } break;
    }
}

int main()
{
    ClientNetworkInterface clientNetwork;

    if (!clientNetwork.Start())
    {
        std::cerr << "Failed to start client peer." << std::endl;
        return 1;
    }

    clientNetwork.OnPacketReceived = HandlePacket;

    std::thread receiveThread(&ClientNetworkInterface::ReceiveLoop, &clientNetwork);

    std::cout << "Client peer initialized successfully." << std::endl;

    std::string testRoomName = "Hello Test Room";
    std::string testMessage = "We're doing it!";

    ClientPacketBuilder packetBuilder;
    PacketData testPacket = packetBuilder.BuildConnectPacket("Test Username");
    clientNetwork.Send(testPacket);
    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Sent %s\n", PacketTypeToString(PacketType_Connect));

    testPacket = packetBuilder.BuildListChatRoomsPacket();
    clientNetwork.Send(testPacket);
    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Sent %s\n", PacketTypeToString(PacketType_ListChatRooms));

    testPacket = packetBuilder.BuildCreateChatRoomPacket("Cool Room");
    clientNetwork.Send(testPacket);
    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Sent %s\n", PacketTypeToString(PacketType_CreateChatRoom));

    testPacket = packetBuilder.BuildJoinChatRoomPacket(0);
    clientNetwork.Send(testPacket);
    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Sent %s\n", PacketTypeToString(PacketType_JoinChatRoom));

    testPacket = packetBuilder.BuildSendMessagePacket(0, "Hello, cool message!");
    clientNetwork.Send(testPacket);
    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Sent %s\n", PacketTypeToString(PacketType_SendMessage));

    testPacket = packetBuilder.BuildLeaveChatRoomPacket(0);
    clientNetwork.Send(testPacket);
    LogDepthConditional(LOG_NETWORK_PACKETS, 0, "Sent %s\n", PacketTypeToString(PacketType_LeaveChatRoom));

    while (true) {}

    clientNetwork.Shutdown();
}