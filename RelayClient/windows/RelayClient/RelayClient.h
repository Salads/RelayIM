#ifndef RELAYCLIENT_H
#define RELAYCLIENT_H

#include <QMainWindow>
#include <QListView>
#include <QPushButton>
#include <QBoxLayout>
#include "ui_RelayClient.h" // Used by status bar

#include "models/QModelManager/QModelManager.h"
#include "models/QChatRoomsModel/QChatRoomsModel.h"
#include "widgets/QChatWidget/QChatWidget.h"
#include "widgets/QConnectionStatus/QConnectionStatus.h"
#include "windows/ChatRoomsDialog/QChatRoomsDialog.h"

class RelayClient : public QMainWindow
{
    Q_OBJECT

public:
    RelayClient(QModelManager* manager, QWidget *parent = nullptr);
    ~RelayClient();

    /// <summary>
    /// Purely a socket-level connect, without registering a username
    /// </summary>
    /// <returns>true if Successful</returns>
    bool tryConnect();

    void setStatusUI(QConnectionStatus::Status status);

    void updateWindowTitle();

private:
    void initializeSignalConnections();
    void setCurrentRoom(RoomID roomID);

public slots:
    void slotJoinRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);
    void slotCreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);
    void slotRoomUpdateUserLeft(RoomID roomID, PeerID peerID);

private:
    Ui::RelayClientClass ui;

    const QSize c_minSize = QSize(650, 393);

    QModelManager* m_manager;

    QListView* m_roomsListView;
    QChatWidget* m_chatWidget;

    QPushButton* m_createOrJoinChatRoomButton;
    QPushButton* m_leaveChatRoomButton;

    QConnectionStatus* m_connectionStatus;
};

#endif // RELAYCLIENT_H

