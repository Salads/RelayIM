#pragma once

#include <QMainWindow>
#include <QListView>
#include <QPushButton>
#include <QBoxLayout>
#include <QErrorMessage>
#include "ui_RelayClient.h"

#include "models/QModelManager/QModelManager.h"
#include "models/QChatRoomsModel/QChatRoomsModel.h"
#include "widgets/QChatWidget/QChatWidget.h"
#include "widgets/QConnectionStatus/QConnectionStatus.h"
#include "windows/QRegisterDialog/QRegisterDialog.h"
#include "windows/ChatRoomsDialog/QChatRoomsDialog.h"

class RelayClient : public QMainWindow
{
    Q_OBJECT

public:
    RelayClient(QWidget *parent = nullptr);
    ~RelayClient();

    void TryConnect();

private:
    void InitializeSignalConnections();
    void SetCurrentRoom(RoomID roomID);

public slots:

    /*
        NOTE(Salads):

        For QModelManager network-releated events. Make sure to connect using Qt::QueuedConnection, as
        model data access MUST happen on the Qt main thread, or else it'll silently fail.
    */
    void Slot_RegisterResponse(PacketResponseReason result, PeerID peerID, std::string username);
    void Slot_JoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);
    void Slot_CreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);

    void Slot_RoomUpdate_Message(RoomID roomID, PeerID peerID, std::string message);
    void Slot_RoomUpdate_FULL(RoomID roomID, std::shared_ptr<std::vector<ChatMessage>> messages);
    void Slot_RoomUpdate_UserJoined(RoomID roomID, PeerID newPeerID, std::string newName);
    void Slot_RoomUpdate_UserLeft(RoomID roomID, PeerID peerID);

private:
    Ui::RelayClientClass ui;

    QModelManager m_manager;

    QListView* m_roomsListView;
    QChatWidget* m_chatWidget;

    QPushButton* m_createOrJoinChatRoomButton;
    QPushButton* m_leaveChatRoomButton;

    QConnectionStatus* m_connectionStatus;

    QRegisterDialog* m_registerDialog;
};

