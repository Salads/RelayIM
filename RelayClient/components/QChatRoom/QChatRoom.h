#ifndef QCHATROOM_H
#define QCHATROOM_H

#include <QPushButton>
#include "ui_QChatRoom.h"
#include <qlabel.h>
#include <qboxlayout.h>

// UI for representing a chat room
class QChatRoom : public QPushButton
{
    Q_OBJECT

public:
    QChatRoom(QWidget *parent = nullptr);
    ~QChatRoom();

    void Initialize(int roomId, const std::string roomName);

private:
    Ui::QChatRoomClass ui;

    QLabel* m_roomNameLabel;
};

#endif // QCHATROOM_H

