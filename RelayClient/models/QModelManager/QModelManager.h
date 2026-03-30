#ifndef QMODELMANAGER_H
#define QMODELMANAGER_H

#include <QObject>
#include <QMutexLocker>
#include <QAbstractItemModel>
#include <QHash>
#include <QScrollBar>

#include "RelayIMClient.h"
#include "models/QChatRoomsModel/QChatRoomsModel.h"
#include "models/QChatRoomMessagesModel/QChatRoomMessagesModel.h"
#include "IRelayIMClientPacketHandler.h"

#include "Logging.h"
#include "PacketResponseReason.h"

class QModelManager : public QObject, IRelayIMClientPacketHandler
{
    Q_OBJECT  // Required for signals/slots

public:
    QModelManager(QObject* parent = nullptr);
    ~QModelManager();

    bool Initialize();
    bool Connect();
    void Shutdown();

    PeerID GetLocalPeerID();
    void SetLocalPeerID(PeerID peerID);

    void AddKnownUser(PeerID peerID, const std::string& username);
    void AddUserToRoom(PeerID peerID, RoomID roomID);
    void AddMessageToRoom(RoomID roomID, PeerID peerID, const std::string& message);

    void RemoveUserFromRoom(PeerID peerID, RoomID roomID);
    void RemoveJoinedChatRoom(RoomID roomID);

    QModelIndex GetChatRoomIdx(RoomID roomID);

    bool HasJoinedRoom(RoomID roomID);
    PacketResponseReason CheckDesiredUsername(const std::string& desiredUsername);

    /// <summary>
    /// Adds a chat room to the QChatRoomsModel, and creates a new QChatRoomMessagesModel for the RoomID given.
    /// If a QChatRoomMessagesModel exists, deletes it first before adding a new one.
    /// </summary>
    /// <param name="roomID">RoomID of new chat room</param>
    /// <param name="roomname">Display roomname of new chat room</param>
    void AddJoinedChatRoom(RoomID roomID, const std::string& roomname);

    QChatRoomsModel* GetModelForRooms();
    QList<ChatMessage>* GetMessagesForRoom(RoomID roomID);

    RelayIMClient* GetClient();

    std::string GetUsernameByPeerID(PeerID peerID);
    std::string GetRoomnameByRoomID(RoomID roomID);

    // Local Client registration result
    void OnRegisterResponse(PacketResponseReason reason, PeerID peerID, std::string username) override;

    // Requested information of all existing chat rooms result
    void OnListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>> chatRooms) override;

    // Local Client join existing chat room result
    void OnJoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName) override;

    // Local Client create chat room result (also joins room)
    void OnCreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName) override;

    // Any Client has sent a message to a chat room
    void OnRoomUpdate_NewMessage(RoomID roomID, PeerID peerID, std::string message) override;

    // Local Client joined a existing chat room with messages/clients
    void OnRoomUpdate_FullUpdate(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages) override;

    // Remote Client has joined a chat room we're in.
    void OnRoomUpdate_UserJoined(RoomID roomID, PeerID newPeerID, std::string newName) override;

    // Remote Client has left a chat room we're in.
    void OnRoomUpdate_UserLeft(RoomID roomID, PeerID peerID) override;
    

signals:
    // NOTE(Salads): Private signals for network data handling ONLY. Use Qt::QueuedConnection to bring data to Qt thread.
    void NetEvent_RegisterResponse(PacketResponseReason result, PeerID peerID, std::string username, QPrivateSignal);
    void NetEvent_JoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName, QPrivateSignal);
    void NetEvent_CreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName, QPrivateSignal);
    void NetEvent_ListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>>, QPrivateSignal);
    void NetEvent_RoomUpdate_Message(RoomID roomID, PeerID peerID, std::string message, QPrivateSignal);
    void NetEvent_RoomUpdate_FULL(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages, QPrivateSignal);
    void NetEvent_RoomUpdate_UserJoined(RoomID roomID, PeerID newPeerID, std::string newName, QPrivateSignal);
    void NetEvent_RoomUpdate_UserLeft(RoomID roomID, PeerID peerID, QPrivateSignal);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Note(Salads): Events for use with UI, with the guarantee that the data is ready (and is on main Qt thread)
    void Event_RegisterResponse(PacketResponseReason result, PeerID peerID, std::string username);
    void Event_JoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);
    void Event_CreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);
    void Event_ListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>>);
    void Event_RoomUpdate_Message(RoomID roomID, PeerID peerID, std::string message);
    void Event_RoomUpdate_FULL(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages);
    void Event_RoomUpdate_UserJoined(RoomID roomID, PeerID newPeerID, std::string newName);
    void Event_RoomUpdate_UserAboutToLeave(RoomID roomID, PeerID peerID); // This event happens BEFORE data alteration, since controls depend on the model that will be deleted.

private slots:
    // NOTE(Salads): Private slots for network data handling ONLY. Use Qt::QueuedConnection to bring data to Qt thread.
    void NetSlot_RegisterResponse(PacketResponseReason result, PeerID peerID, std::string username, QPrivateSignal);
    void NetSlot_JoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName, QPrivateSignal);
    void NetSlot_CreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName, QPrivateSignal);
    void NetSlot_ListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>>, QPrivateSignal);
    void NetSlot_RoomUpdate_Message(RoomID roomID, PeerID peerID, std::string message, QPrivateSignal);
    void NetSlot_RoomUpdate_FULL(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages, QPrivateSignal);
    void NetSlot_RoomUpdate_UserJoined(RoomID roomID, PeerID newPeerID, std::string newName, QPrivateSignal);
    void NetSlot_RoomUpdate_UserLeft(RoomID roomID, PeerID peerID, QPrivateSignal);

private:
    void InitializeClientCallbacks();
    void DeleteRoomMessagesModel(RoomID roomID);

private:
    RelayIMClient m_client;

    bool m_callbacksInitialized = false;

    PeerID m_localPeerID; 

    QHash<PeerID, std::string> m_knownUsers;
    QHash<PeerID, QSet<RoomID>> m_userRooms;

    QChatRoomsModel m_joinedChatRoomsModel;
    QHash<RoomID, QList<ChatMessage>> m_chatRoomMessages;
};

#endif // QMODELMANAGER_H