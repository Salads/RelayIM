#pragma once

#include <QWidget>
#include <qscrollarea.h>
#include <qboxlayout.h>
#include "ui_QChatRooms.h"

class QChatRooms : public QWidget
{
    Q_OBJECT

public:
    QChatRooms(QWidget *parent = nullptr);
    ~QChatRooms();

    void addRoom(int roomId, const std::string roomName);

private:
    Ui::QChatRoomsClass ui;

    QWidget* m_container;
    QVBoxLayout* m_containerLayout;
    QScrollArea* m_scrollArea;
};

