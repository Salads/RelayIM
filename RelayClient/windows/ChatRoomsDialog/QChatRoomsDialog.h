#pragma once

#include <QDialog>
#include <QBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QLineEdit>
#include <QPushButton>
#include <QListView>
#include <QGroupBox>
#include <QMessageBox>
#include "ui_QChatRoomsDialog.h"
#include "PacketResponseReason.h"

#include "models/QModelManager/QModelManager.h"
#include "models/QChatRoomsModel/QChatRoomsModel.h"

class QChatRoomsDialog : public QDialog
{
    Q_OBJECT

public:
    QChatRoomsDialog(QModelManager* manager, QWidget *parent = nullptr);
    ~QChatRoomsDialog();

    PacketResponseReason CheckRoomname(const std::string& newRoomname);

private:
    void Slot_JoinRoomResponse(PacketResponseReason reason, RoomID roomID, std::string newRoomName);
    void Slot_JoinableRoomSelected(const QModelIndex& current, const QModelIndex& prev);
    void Slot_JoinRoomButtonClicked(bool checked);
    void Slot_JoinableChatRoomsReceived(std::shared_ptr<std::vector<ChatRoomInfo>> chatRooms);
    void Slot_CreateRoomButtonClicked(bool checked);
    void Slot_CreateRoomResponse(PacketResponseReason reason, RoomID newRoomID, std::string newChatRoomName);

private:
    Ui::QChatRoomsDialogClass ui;

    QModelManager* m_manager;

    QChatRoomsModel m_model;

    QListView* m_joinableRoomsListView;
    QPushButton* m_joinRoomButton;

    QLineEdit* m_createRoomLineEdit;
    QLabel* m_createRoomLabel;
    QLabel* m_createRoomErrorLabel;

    QPushButton* m_createButton;
};

