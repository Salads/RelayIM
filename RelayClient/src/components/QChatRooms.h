#pragma once

#include <QWidget>
#include "ui_QChatRooms.h"

class QChatRooms : public QWidget
{
    Q_OBJECT

public:
    QChatRooms(QWidget *parent = nullptr);
    ~QChatRooms();

private:
    Ui::QChatRoomsClass ui;
};

