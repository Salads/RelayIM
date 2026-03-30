#ifndef SIMPLECLIENTTESTPACKETHANDLER_H
#define SIMPLECLIENTTESTPACKETHANDLER_H

#include <vector>
#include <string>
#include <atomic>
#include <memory>

#include "RelayIMClientAbstractPacketHandler.h"
#include "RelayImClient.h"
#include "Util.h"
#include "RoomID.h"
#include "PeerID.h"

class SimpleClientTestPacketHandler : public RelayIMClientAbstractPacketHandler
{
public:
    SimpleClientTestPacketHandler();
    ~SimpleClientTestPacketHandler();

    bool initialize();
    bool connectToServer();

    /// <summary>
    /// Tests a sequence with no unexpected inputs.
    /// </summary>
    /// <returns></returns>
    bool testStandardSequence();

// Network callbacks
public:
    // Local Client registration result
    void onRegisterResponse(PacketResponseReason reason, PeerID peerID, std::string username) override;

    // Requested information of all existing chat rooms result
    void onListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>> chatrooms) override;

    // Local Client join existing chat room result
    void onJoinRoomResponse(PacketResponseReason reason, RoomID roomID, std::string roomname) override;

    // Local Client create chat room result (also joins room)
    void onCreateRoomResponse(PacketResponseReason reason, RoomID roomID, std::string roomname) override;

    // Any Client has sent a message to a chat room
    void onRoomUpdateNewMessage(RoomID roomID, PeerID peerID, std::string message) override;

    // Local Client joined a existing chat room with messages/clients
    void onRoomUpdateFullUpdate(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages) override;

    // Remote Client has joined a chat room we're in.
    void onRoomUpdateUserJoined(RoomID roomID, PeerID peerID, std::string username) override;

    // Remote Client has left a chat room we're in.
    void onRoomUpdateUserLeft(RoomID roomID, PeerID peerID) override;

private:
    void waitForNextSequenceStep();
    void allowNextSequenceStep();
    bool checkSequenceStep(const char* funcname);

private:
    RelayIMClient m_client;

    std::vector<std::string> m_expectedIncomingPacketSequence;
    size_t m_currentSequenceIdx = 0;

    std::atomic_bool m_sequenceFlag = false;
    std::atomic_bool m_success = false;
};

#endif // SIMPLECLIENTTESTPACKETHANDLER_H