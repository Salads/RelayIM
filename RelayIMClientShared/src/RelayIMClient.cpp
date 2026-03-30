#include "RelayIMClient.h"

RelayIMClient::RelayIMClient(RelayIMClientAbstractPacketHandler* handler)
    : m_clientNetwork(this), m_handler(handler)
{}

bool RelayIMClient::initialize()
{
    if (!m_clientNetwork.initializeInterface())
    {
        return false;
    }

    return true;
}

bool RelayIMClient::connectToServer()
{
    return m_clientNetwork.connectToServer();
}

void RelayIMClient::shutdownClient()
{
    m_clientNetwork.shutdownInterface();
}

void RelayIMClient::onServerDisconnected()
{
    // TODO(Salads): onServerDisconnected
}

void RelayIMClient::onPacketReceived(std::unique_ptr<NetworkPacket> serverPacket)
{
    std::unique_ptr<NetworkPacket> packet = std::move(serverPacket);
    PacketReader reader(packet.get());
    PacketHeader header; reader.readHeader(header);

    Log::get()->conditionalWriteLine(LOG_NETWORK_PACKET_TYPES, "RECV(%s)", packetTypeToString(header.m_packetType));

    switch (header.m_packetType)
    {
    case PacketType_ConnectResponse:
    {
        PacketResponseReason responseReason;
        PeerID peerID;
        std::string username;

        reader.readPacketResponseReason(responseReason);
        if (responseReason == PacketResponseReason::Success)
        {
            reader.readPeerID(peerID);
            reader.readString(username);
        }
        
        m_handler->onRegisterResponse(responseReason, peerID, username);

    } break;
    case PacketType_ListChatRooms_Result:
    {
        std::unique_ptr<std::vector<ChatRoomInfo>> rooms = std::make_unique<std::vector<ChatRoomInfo>>();

        uint16_t nRooms = 0; reader.readUInt16(nRooms);
        for (int i = 0; i < nRooms; i++)
        {
            RoomID roomID; reader.readRoomID(roomID);
            std::string roomName; reader.readString(roomName);

            rooms->emplace_back(roomID, roomName);
        }

        m_handler->onListChatRoomsResponse(std::move(rooms));

    } break;
    case PacketType_JoinChatRoomResponse:
    {
        PacketResponseReason result; reader.readPacketResponseReason(result);
        RoomID newRoomID;
        std::string newRoomname;

        if (result == PacketResponseReason::Success)
        {
            reader.readRoomID(newRoomID);
            reader.readString(newRoomname);
        }

        m_handler->onJoinRoomResponse(result, newRoomID, newRoomname);

    } break;
    case PacketType_CreateChatRoomResponse:
    {
        PacketResponseReason result; reader.readPacketResponseReason(result);
        RoomID newRoomID;
        std::string newRoomname;

        if (result == PacketResponseReason::Success)
        {
            reader.readRoomID(newRoomID);
            reader.readString(newRoomname);
        }

        m_handler->onCreateRoomResponse(result, newRoomID, newRoomname);

    } break;
    case PacketType_RoomUpdate_MSG:
    {
        RoomID roomID;   reader.readRoomID(roomID);
        PeerID peerID;   reader.readPeerID(peerID);
        std::string message; reader.readString(message);

        m_handler->onRoomUpdateNewMessage(roomID, peerID, message);
        
    } break;
    case PacketType_RoomUpdate_FULL:
    {
        RoomID roomID;   reader.readRoomID(roomID);

        // ARRAY of users in chat room (some users might not have sent a message)
        uint16_t nUsers = 0; reader.readUInt16(nUsers);
        for (int i = 0; i < nUsers; i++)
        {
            PeerID userID; reader.readPeerID(userID);
            std::string username; reader.readString(username);

            m_handler->onRoomUpdateUserJoined(roomID, userID, username);
        }

        std::unique_ptr<std::vector<ChatMessage>> messages = std::make_unique<std::vector<ChatMessage>>();
        uint16_t nMessages = 0; reader.readUInt16(nMessages);
        for (int i = 0; i < nMessages; i++)
        {
            PeerID userID; reader.readPeerID(userID);
            std::string message; reader.readString(message);

            messages->emplace_back(userID, message);
        }

        m_handler->onRoomUpdateFullUpdate(roomID, std::move(messages));

    } break;
    case PacketType_RoomUpdate_UserJoined:
    {
        RoomID roomID; reader.readRoomID(roomID);
        PeerID peerID; reader.readPeerID(peerID);
        std::string username; reader.readString(username);

        m_handler->onRoomUpdateUserJoined(roomID, peerID, username);

    } break;
    case PacketType_RoomUpdate_UserLeft:
    {
        RoomID roomID; reader.readRoomID(roomID);
        PeerID peerID; reader.readPeerID(peerID);

        m_handler->onRoomUpdateUserLeft(roomID, peerID);

    } break;
    default:
    {

    } break;
    }
}

void RelayIMClient::sendConnect(std::string desiredUsername)
{
    PacketData packetData;
    PacketWriter writer(packetData);
    writer.writeHeader(PacketType_Connect);
    writer.writeString(desiredUsername);
    writer.finalize();

    m_clientNetwork.sendPacket(packetData);
}

void RelayIMClient::sendRequestAllChatRooms()
{
    PacketData packetData;
    PacketWriter writer(packetData);
    writer.writeHeader(PacketType_ListChatRooms);
    writer.finalize();

    m_clientNetwork.sendPacket(packetData);
}

void RelayIMClient::sendJoinChatRoom(RoomID roomID)
{
    PacketData packetData;
    PacketWriter writer(packetData);
    writer.writeHeader(PacketType_JoinChatRoom);
    writer.writeRoomId(roomID);
    writer.finalize();

    m_clientNetwork.sendPacket(packetData);
}

void RelayIMClient::sendCreateChatRoom(std::string roomName)
{
    PacketData packetData;
    PacketWriter writer(packetData);
    writer.writeHeader(PacketType_CreateChatRoom);
    writer.writeString(roomName);
    writer.finalize();

    m_clientNetwork.sendPacket(packetData);
}

void RelayIMClient::sendLeaveChatRoom(RoomID roomID)
{
    PacketData packetData;
    PacketWriter writer(packetData);
    writer.writeHeader(PacketType_LeaveChatRoom);
    writer.writeRoomId(roomID);
    writer.finalize();

    m_clientNetwork.sendPacket(packetData);
}

void RelayIMClient::sendMessageToRoom(RoomID roomID, std::string message)
{
    Log::get()->conditionalWriteLine(LOG_UI, "Sending '%s' to Room %u", message, roomID);

    PacketData packetData;
    PacketWriter writer(packetData);
    writer.writeHeader(PacketType_SendMessage);
    writer.writeRoomId(roomID);
    writer.writeString(message);
    writer.finalize();

    m_clientNetwork.sendPacket(packetData);
}
