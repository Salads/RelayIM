#pragma once

#include <QWidget>
#include <QButtonGroup>
#include <qscrollarea.h>
#include <qboxlayout.h>
#include "src/components/QChatRoom.h"
#include "ui_QChatRooms.h"

#include <vector>

class QChatRooms : public QWidget
{
    Q_OBJECT

public:
    QChatRooms(QWidget *parent = nullptr);
    ~QChatRooms();

    void addRoom(int roomId, const std::string roomName);

private:
    Ui::QChatRoomsClass ui;

    QButtonGroup* m_buttonGroup;
    QWidget* m_container;
    QVBoxLayout* m_containerLayout;
    QScrollArea* m_scrollArea;
};

