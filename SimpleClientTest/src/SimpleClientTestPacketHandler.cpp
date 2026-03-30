#include "SimpleClientTestPacketHandler.h"

SimpleClientTestPacketHandler::SimpleClientTestPacketHandler()
    :m_client(this)
{
    m_expectedIncomingPacketSequence =
    {
        "SimpleClientTestPacketHandler::onRegisterResponse",
        "SimpleClientTestPacketHandler::onCreateRoomResponse", //r1
        "SimpleClientTestPacketHandler::onCreateRoomResponse", //r2
        "SimpleClientTestPacketHandler::onRoomUpdateNewMessage", // "test" -> r1
        "SimpleClientTestPacketHandler::onRoomUpdateUserLeft", // r1
        "SimpleClientTestPacketHandler::onListChatRoomsResponse", // r1
        "SimpleClientTestPacketHandler::onJoinRoomResponse", // r1
        "SimpleClientTestPacketHandler::onRoomUpdateFullUpdate" // 1 msg
    };
}

SimpleClientTestPacketHandler::~SimpleClientTestPacketHandler()
{
    m_client.shutdownClient();
}

bool SimpleClientTestPacketHandler::initialize()
{
    return m_client.initialize();
}

bool SimpleClientTestPacketHandler::connectToServer()
{
    return m_client.connectToServer();
}

void SimpleClientTestPacketHandler::waitForNextSequenceStep()
{
    m_sequenceFlag = false;
    while(!m_sequenceFlag && m_success) {}

    m_currentSequenceIdx++;
}

void SimpleClientTestPacketHandler::allowNextSequenceStep() 
{
    m_sequenceFlag = true;
}

bool SimpleClientTestPacketHandler::testStandardSequence()
{
    m_success = true;
    m_currentSequenceIdx = 0;

    m_client.sendConnect("TestUsername");          waitForNextSequenceStep(); if(!m_success) { return false; }
    m_client.sendCreateChatRoom("r1");             waitForNextSequenceStep(); if(!m_success) { return false; }
    m_client.sendCreateChatRoom("r2");             waitForNextSequenceStep(); if(!m_success) { return false; }
    m_client.sendMessageToRoom(RoomID(0), "test"); waitForNextSequenceStep(); if(!m_success) { return false; }
    m_client.sendLeaveChatRoom(RoomID(0));         waitForNextSequenceStep(); if(!m_success) { return false; }
    m_client.sendRequestAllChatRooms();            waitForNextSequenceStep(); if(!m_success) { return false; }
    m_client.sendJoinChatRoom(RoomID(0));          waitForNextSequenceStep(); if(!m_success) { return false; }

    return m_success;
}

bool SimpleClientTestPacketHandler::checkSequenceStep(const char* funcname)
{
    if(m_expectedIncomingPacketSequence[m_currentSequenceIdx] != funcname)
    {
        return false;
    }

    return true;
}

void SimpleClientTestPacketHandler::onRegisterResponse(PacketResponseReason reason, PeerID peerID, std::string username) 
{
    if(!checkSequenceStep(__FUNCTION__) || reason != PacketResponseReason::Success || peerID != 0 || username != "TestUsername")
    {
        m_success = false;
        std::cout << "Failed: " << __FUNCTION__ << "Reason: " << responseTypeToString(reason) << "PeerID: " << peerID << "Username: " << username << std::endl;
    }
    else
    {
        allowNextSequenceStep();
    }
}

void SimpleClientTestPacketHandler::onCreateRoomResponse(PacketResponseReason reason, RoomID roomID, std::string roomname)
{
    if(!checkSequenceStep(__FUNCTION__) || reason != PacketResponseReason::Success)
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
    else if(m_currentSequenceIdx == 1 && (roomID != 0 || roomname != "r1"))
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
    else if(m_currentSequenceIdx == 2 && (roomID != 1 || roomname != "r2"))
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
    else
    {
        allowNextSequenceStep();
    }
}

void SimpleClientTestPacketHandler::onRoomUpdateNewMessage(RoomID roomID, PeerID peerID, std::string message)
{
    if(!checkSequenceStep(__FUNCTION__) || roomID != 0 || peerID != 0 || message != "test")
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
    else
    {
        allowNextSequenceStep();
    }
}

void SimpleClientTestPacketHandler::onRoomUpdateUserLeft(RoomID roomID, PeerID peerID)
{
    if(!checkSequenceStep(__FUNCTION__) || roomID != 0 || peerID != 0)
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
    else
    {
        allowNextSequenceStep();
    }
}

void SimpleClientTestPacketHandler::onListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>> chatrooms) 
{
    if(!checkSequenceStep(__FUNCTION__) || chatrooms->size() != 2)
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
    else
    {
        allowNextSequenceStep();
    }
}

void SimpleClientTestPacketHandler::onJoinRoomResponse(PacketResponseReason reason, RoomID roomID, std::string roomname) 
{
    if(!checkSequenceStep(__FUNCTION__) || reason != PacketResponseReason::Success || roomID != 0 || roomname != "r1")
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
    else
    {
        allowNextSequenceStep();
    }
}

void SimpleClientTestPacketHandler::onRoomUpdateFullUpdate(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages) 
{
    if(!checkSequenceStep(__FUNCTION__) || roomID != 0 || messages->size() != 1 || messages->at(0).m_message != "test")
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
}

void SimpleClientTestPacketHandler::onRoomUpdateUserJoined(RoomID roomID, PeerID peerID, std::string username) 
{
    if(!checkSequenceStep(__FUNCTION__))
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
    else
    {
        allowNextSequenceStep();
    }
}
