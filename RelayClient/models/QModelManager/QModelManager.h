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
#include "RelayIMClientAbstractPacketHandler.h"

#include "Logging.h"
#include "PacketResponseReason.h"

class QModelManager : public QObject, RelayIMClientAbstractPacketHandler
{
    Q_OBJECT  // Required for signals/slots

public:
    QModelManager(QObject* parent = nullptr);
    ~QModelManager();

    bool initialize();
    bool connectToServer();
    void shutdown();

    PeerID getLocalPeerId();
    void setLocalPeerId(PeerID peerID);

    void addKnownUser(PeerID peerID, const std::string& username);
    void addUserToRoom(PeerID peerID, RoomID roomID);
    void addMessageToRoom(RoomID roomID, PeerID peerID, const std::string& message);

    void removeUserFromRoom(PeerID peerID, RoomID roomID);
    void removeJoinedChatRoom(RoomID roomID);

    QModelIndex getChatRoomIdx(RoomID roomID);

    bool hasJoinedRoom(RoomID roomID);
    PacketResponseReason checkDesiredUsername(const std::string& desiredUsername);

    /// <summary>
    /// Adds a chat room to the QChatRoomsModel, and creates a new QChatRoomMessagesModel for the RoomID given.
    /// If a QChatRoomMessagesModel exists, deletes it first before adding a new one.
    /// </summary>
    /// <param name="roomID">RoomID of new chat room</param>
    /// <param name="roomname">Display roomname of new chat room</param>
    void addJoinedChatRoom(RoomID roomID, const std::string& roomname);

    QChatRoomsModel* getModelForRooms();
    QList<ChatMessage>* getMessagesForRoom(RoomID roomID);

    RelayIMClient* getClient();

    std::string getUsernameByPeerId(PeerID peerID);
    std::string getRoomnameByRoomId(RoomID roomID);

    // Local Client registration result
    void onRegisterResponse(PacketResponseReason reason, PeerID peerID, std::string username) override;

    // Requested information of all existing chat rooms result
    void onListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>> chatRooms) override;

    // Local Client join existing chat room result
    void onJoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName) override;

    // Local Client create chat room result (also joins room)
    void onCreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName) override;

    // Any Client has sent a message to a chat room
    void onRoomUpdateNewMessage(RoomID roomID, PeerID peerID, std::string message) override;

    // Local Client joined a existing chat room with messages/clients
    void onRoomUpdateFullUpdate(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages) override;

    // Remote Client has joined a chat room we're in.
    void onRoomUpdateUserJoined(RoomID roomID, PeerID newPeerID, std::string newName) override;

    // Remote Client has left a chat room we're in.
    void onRoomUpdateUserLeft(RoomID roomID, PeerID peerID) override;
    

signals:
    // NOTE(Salads): Private signals for network data handling ONLY. Use Qt::QueuedConnection to bring data to Qt thread.
    void netEventRegisterResponse(PacketResponseReason result, PeerID peerID, std::string username, QPrivateSignal);
    void netEventJoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName, QPrivateSignal);
    void netEventCreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName, QPrivateSignal);
    void netEventListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>>, QPrivateSignal);
    void netEventRoomUpdateMessage(RoomID roomID, PeerID peerID, std::string message, QPrivateSignal);
    void netEventRoomUpdateFull(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages, QPrivateSignal);
    void netEventRoomUpdateUserJoined(RoomID roomID, PeerID newPeerID, std::string newName, QPrivateSignal);
    void netEventRoomUpdateUserLeft(RoomID roomID, PeerID peerID, QPrivateSignal);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Note(Salads): Events for use with UI, with the guarantee that the data is ready (and is on main Qt thread)
    void eventRegisterResponse(PacketResponseReason result, PeerID peerID, std::string username);
    void eventJoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);
    void eventCreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);
    void eventListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>>);
    void eventRoomUpdateMessage(RoomID roomID, PeerID peerID, std::string message);
    void eventRoomUpdateFull(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages);
    void eventRoomUpdateUserJoined(RoomID roomID, PeerID newPeerID, std::string newName);
    void eventRoomUpdateUserAboutToLeave(RoomID roomID, PeerID peerID); // This event happens BEFORE data alteration, since controls depend on the model that will be deleted.

private slots:
    // NOTE(Salads): Private slots for network data handling ONLY. Use Qt::QueuedConnection to bring data to Qt thread.
    void netSlotRegisterResponse(PacketResponseReason result, PeerID peerID, std::string username, QPrivateSignal);
    void netSlotJoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName, QPrivateSignal);
    void netSlotCreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName, QPrivateSignal);
    void netSlotListChatRoomsResponse(std::shared_ptr<std::vector<ChatRoomInfo>>, QPrivateSignal);
    void netSlotRoomUpdateMessage(RoomID roomID, PeerID peerID, std::string message, QPrivateSignal);
    void netSlotRoomUpdateFull(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages, QPrivateSignal);
    void netSlotRoomUpdateUserJoined(RoomID roomID, PeerID newPeerID, std::string newName, QPrivateSignal);
    void netSlotRoomUpdateUserLeft(RoomID roomID, PeerID peerID, QPrivateSignal);

private:
    void initializeClientCallbacks();
    void deleteRoomMessagesModel(RoomID roomID);

private:
    RelayIMClient m_client;

    bool m_callbacksInitialized = false;

    PeerID m_localPeerId; 

    QHash<PeerID, std::string> m_knownUsers;
    QHash<PeerID, QSet<RoomID>> m_userRooms;

    QChatRoomsModel m_joinedChatRoomsModel;
    QHash<RoomID, QList<ChatMessage>> m_chatRoomMessages;
};

#endif // QMODELMANAGER_H