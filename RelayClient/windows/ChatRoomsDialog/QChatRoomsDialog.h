#ifndef QCHATROOMSDIALOG_H
#define QCHATROOMSDIALOG_H

#include <QDialog>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListView>
#include <QGroupBox>
#include <QMessageBox>

#include "models/QModelManager/QModelManager.h"
#include "models/QChatRoomsModel/QChatRoomsModel.h"

#include "PacketResponseReason.h"

class QChatRoomsDialog : public QDialog
{
    Q_OBJECT

public:
    QChatRoomsDialog(QModelManager* manager, QWidget *parent = nullptr);
    ~QChatRoomsDialog();

    PacketResponseReason checkRoomname(const std::string& newRoomname);

private slots:
    void slotJoinRoomResponse(PacketResponseReason reason, RoomID roomID, std::string newRoomName);
    void slotJoinableRoomSelected(const QModelIndex& current, const QModelIndex& prev);
    void slotJoinRoomButtonClicked(bool checked);
    void slotJoinableChatRoomsReceived(std::shared_ptr<std::vector<ChatRoomInfo>> chatRooms);
    void slotCreateRoomButtonClicked(bool checked);
    void slotCreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);

private:
    QModelManager* m_manager;

    QChatRoomsModel m_model;

    QListView* m_joinableRoomsListView;
    QPushButton* m_joinRoomButton;

    QLineEdit* m_createRoomLineEdit;
    QLabel* m_createRoomLabel;
    QLabel* m_createRoomErrorLabel;

    QPushButton* m_createButton;
};

#endif // QCHATROOMSDIALOG_H

