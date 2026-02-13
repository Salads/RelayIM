#pragma once

#include <QWidget>
#include "ui_QChatRoom.h"

#include <qlabel.h>

// UI for representing a chat room
class QChatRoom : public QWidget
{
    Q_OBJECT

public:
    QChatRoom(QWidget *parent = nullptr);
    ~QChatRoom();

    void Initialize(int roomId, const std::string roomName);

private:
    Ui::QChatRoomClass ui;

    QLabel* m_roomNameLabel;
    int m_roomId;
};

