#include "SimpleClientTestPacketHandler.h"

SimpleClientTestPacketHandler::SimpleClientTestPacketHandler()
    :m_client(this)
{
    m_expectedIncomingPacketSequence =
    {
        "SimpleClientTestPacketHandler::OnRegisterResponse",
        "SimpleClientTestPacketHandler::OnCreateRoomResponse", //r1
        "SimpleClientTestPacketHandler::OnCreateRoomResponse", //r2
        "SimpleClientTestPacketHandler::OnRoomUpdate_NewMessage", // "test" -> r1
        "SimpleClientTestPacketHandler::OnRoomUpdate_UserLeft", // r1
        "SimpleClientTestPacketHandler::OnListChatRoomsResponse", // r1
        "SimpleClientTestPacketHandler::OnJoinRoomResponse", // r1
        "SimpleClientTestPacketHandler::OnRoomUpdate_FullUpdate" // 1 msg
    };
}

SimpleClientTestPacketHandler::~SimpleClientTestPacketHandler()
{
    m_client.Shutdown();
}

bool SimpleClientTestPacketHandler::Initialize()
{
    return m_client.Initialize();
}

bool SimpleClientTestPacketHandler::Connect()
{
    return m_client.Connect();
}

void SimpleClientTestPacketHandler::WaitForNextSequenceStep()
{
    m_sequenceFlag = false;
    while(!m_sequenceFlag && m_success) {}

    m_currentSequenceIdx++;
}

void SimpleClientTestPacketHandler::AllowNextSequenceStep() 
{
    m_sequenceFlag = true;
}

bool SimpleClientTestPacketHandler::TestStandardSequence()
{
    m_success = true;
    m_currentSequenceIdx = 0;

    m_client.SendConnect("TestUsername");  WaitForNextSequenceStep(); if(!m_success) { return false; }
    m_client.SendCreateChatRoom("r1");     WaitForNextSequenceStep(); if(!m_success) { return false; }
    m_client.SendCreateChatRoom("r2");     WaitForNextSequenceStep(); if(!m_success) { return false; }
    m_client.SendMessageToRoom(0, "test"); WaitForNextSequenceStep(); if(!m_success) { return false; }
    m_client.SendLeaveChatRoom(0);         WaitForNextSequenceStep(); if(!m_success) { return false; }
    m_client.SendRequestAllChatRooms();    WaitForNextSequenceStep(); if(!m_success) { return false; }
    m_client.SendJoinChatRoom(0);          WaitForNextSequenceStep(); if(!m_success) { return false; }

    return m_success;
}

bool SimpleClientTestPacketHandler::CheckSequenceStep(const char* funcname)
{
    if(m_expectedIncomingPacketSequence[m_currentSequenceIdx] != funcname)
    {
        return false;
    }

    return true;
}

void SimpleClientTestPacketHandler::OnRegisterResponse(PacketResponseReason reason, PeerID peerID, std::string username) 
{
    if(!CheckSequenceStep(__FUNCTION__) || reason != PacketResponseReason::Success || peerID != 0 || username != "TestUsername")
    {
        m_success = false;
        std::cout << "Failed: " << __FUNCTION__ << "Reason: " << ResponseTypeToString(reason) << "PeerID: " << peerID << "Username: " << username << std::endl;
    }
    else
    {
        AllowNextSequenceStep();
    }
}

void SimpleClientTestPacketHandler::OnCreateRoomResponse(PacketResponseReason reason, RoomID roomID, std::string roomname)
{
    if(!CheckSequenceStep(__FUNCTION__) || reason != PacketResponseReason::Success)
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
        AllowNextSequenceStep();
    }
}

void SimpleClientTestPacketHandler::OnRoomUpdate_NewMessage(RoomID roomID, PeerID peerID, std::string message)
{
    if(!CheckSequenceStep(__FUNCTION__) || roomID != 0 || peerID != 0 || message != "test")
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
    else
    {
        AllowNextSequenceStep();
    }
}

void SimpleClientTestPacketHandler::OnRoomUpdate_UserLeft(RoomID roomID, PeerID peerID)
{
    if(!CheckSequenceStep(__FUNCTION__) || roomID != 0 || peerID != 0)
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
    else
    {
        AllowNextSequenceStep();
    }
}

void SimpleClientTestPacketHandler::OnListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>> chatrooms) 
{
    if(!CheckSequenceStep(__FUNCTION__) || chatrooms->size() != 2)
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
    else
    {
        AllowNextSequenceStep();
    }
}

void SimpleClientTestPacketHandler::OnJoinRoomResponse(PacketResponseReason reason, RoomID roomID, std::string roomname) 
{
    if(!CheckSequenceStep(__FUNCTION__) || reason != PacketResponseReason::Success || roomID != 0 || roomname != "r1")
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
    else
    {
        AllowNextSequenceStep();
    }
}

void SimpleClientTestPacketHandler::OnRoomUpdate_FullUpdate(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages) 
{
    if(!CheckSequenceStep(__FUNCTION__) || roomID != 0 || messages->size() != 1 || messages->at(0).m_message != "test")
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
}

void SimpleClientTestPacketHandler::OnRoomUpdate_UserJoined(RoomID roomID, PeerID peerID, std::string username) 
{
    if(!CheckSequenceStep(__FUNCTION__))
    {
        m_success = false;
        std::cout << "Failed on " << __FUNCTION__ << std::endl;
    }
    else
    {
        AllowNextSequenceStep();
    }
}
