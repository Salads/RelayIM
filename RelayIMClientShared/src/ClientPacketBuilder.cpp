#include "ClientPacketBuilder.h"

PacketData ClientPacketBuilder::BuildConnectPacket(std::string userName)
{
    PacketData result;
    PacketWriter writer(result);

    writer.writeHeader(PacketType_Connect);
    writer.writeString(userName);
    writer.finalize();

    return result;
}

PacketData ClientPacketBuilder::BuildListChatRoomsPacket()
{
    PacketData result;
    PacketWriter writer(result);

    writer.writeHeader(PacketType_ListChatRooms);
    writer.finalize();

    return result;
}

PacketData ClientPacketBuilder::BuildJoinChatRoomPacket(RoomID roomID)
{
    PacketData result;
    PacketWriter writer(result);

    writer.writeHeader(PacketType_JoinChatRoom);
    writer.writeRoomId(roomID);
    writer.finalize();

    return result;
}

PacketData ClientPacketBuilder::BuildCreateChatRoomPacket(std::string roomName)
{
    PacketData result;
    PacketWriter writer(result);

    writer.writeHeader(PacketType_CreateChatRoom);
    writer.writeString(roomName);
    writer.finalize();

    return result;
}

PacketData ClientPacketBuilder::BuildLeaveChatRoomPacket(RoomID roomID)
{
    PacketData result;
    PacketWriter writer(result);

    writer.writeHeader(PacketType_LeaveChatRoom);
    writer.writeRoomId(roomID);
    writer.finalize();

    return result;
}

PacketData ClientPacketBuilder::BuildSendMessagePacket(RoomID roomID, std::string message)
{
    PacketData result;
    PacketWriter writer(result);

    writer.writeHeader(PacketType_SendMessage);
    writer.writeRoomId(roomID);
    writer.writeString(message);
    writer.finalize();

    return result;
}
