#ifndef SIMPLECLIENTTESTPACKETHANDLER_H
#define SIMPLECLIENTTESTPACKETHANDLER_H

#include "IRelayIMClientPacketHandler.h"
#include "RelayImClient.h"
#include "Util.h"
#include "RoomID.h"
#include "PeerID.h"

#include <vector>
#include <string>
#include <atomic>
#include <memory>

class SimpleClientTestPacketHandler : public IRelayIMClientPacketHandler
{
public:
    SimpleClientTestPacketHandler();
    ~SimpleClientTestPacketHandler();

    bool Initialize();
    bool Connect();

    /// <summary>
    /// Tests a sequence with no unexpected inputs.
    /// </summary>
    /// <returns></returns>
    bool TestStandardSequence();

// Network callbacks
public:
    // Local Client registration result
    void OnRegisterResponse(PacketResponseReason reason, PeerID peerID, std::string username) override;

    // Requested information of all existing chat rooms result
    void OnListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>> chatrooms) override;

    // Local Client join existing chat room result
    void OnJoinRoomResponse(PacketResponseReason reason, RoomID roomID, std::string roomname) override;

    // Local Client create chat room result (also joins room)
    void OnCreateRoomResponse(PacketResponseReason reason, RoomID roomID, std::string roomname) override;

    // Any Client has sent a message to a chat room
    void OnRoomUpdate_NewMessage(RoomID roomID, PeerID peerID, std::string message) override;

    // Local Client joined a existing chat room with messages/clients
    void OnRoomUpdate_FullUpdate(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages) override;

    // Remote Client has joined a chat room we're in.
    void OnRoomUpdate_UserJoined(RoomID roomID, PeerID peerID, std::string username) override;

    // Remote Client has left a chat room we're in.
    void OnRoomUpdate_UserLeft(RoomID roomID, PeerID peerID) override;

private:
    void WaitForNextSequenceStep();
    void AllowNextSequenceStep();
    bool CheckSequenceStep(const char* funcname);

private:
    RelayIMClient m_client;

    std::vector<std::string> m_expectedIncomingPacketSequence;
    size_t m_currentSequenceIdx = 0;

    std::atomic_bool m_sequenceFlag = false;
    std::atomic_bool m_success = false;
};

#endif // SIMPLECLIENTTESTPACKETHANDLER_H