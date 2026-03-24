#include "ClientPacketBuilder.h"
#include "PacketWriter.h"

PacketData ClientPacketBuilder::BuildConnectPacket(std::string userName)
{
    PacketData result;
    PacketWriter writer(result);

    writer.WriteHeader(PacketType_Connect);
    writer.WriteString(userName);
    writer.Finalize();

    return result;
}

PacketData ClientPacketBuilder::BuildListChatRoomsPacket()
{
    PacketData result;
    PacketWriter writer(result);

    writer.WriteHeader(PacketType_ListChatRooms);
    writer.Finalize();

    return result;
}

PacketData ClientPacketBuilder::BuildJoinChatRoomPacket(RoomID roomID)
{
    PacketData result;
    PacketWriter writer(result);

    writer.WriteHeader(PacketType_JoinChatRoom);
    writer.WriteRoomID(roomID);
    writer.Finalize();

    return result;
}

PacketData ClientPacketBuilder::BuildCreateChatRoomPacket(std::string roomName)
{
    PacketData result;
    PacketWriter writer(result);

    writer.WriteHeader(PacketType_CreateChatRoom);
    writer.WriteString(roomName);
    writer.Finalize();

    return result;
}

PacketData ClientPacketBuilder::BuildLeaveChatRoomPacket(RoomID roomID)
{
    PacketData result;
    PacketWriter writer(result);

    writer.WriteHeader(PacketType_LeaveChatRoom);
    writer.WriteRoomID(roomID);
    writer.Finalize();

    return result;
}

PacketData ClientPacketBuilder::BuildSendMessagePacket(RoomID roomID, std::string message)
{
    PacketData result;
    PacketWriter writer(result);

    writer.WriteHeader(PacketType_SendMessage);
    writer.WriteRoomID(roomID);
    writer.WriteString(message);
    writer.Finalize();

    return result;
}
