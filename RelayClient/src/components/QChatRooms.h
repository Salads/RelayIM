#pragma once

#include <QWidget>
#include <qscrollarea.h>
#include "ui_QChatRooms.h"

class QChatRooms : public QWidget
{
    Q_OBJECT

public:
    QChatRooms(QWidget *parent = nullptr);
    ~QChatRooms();

private:
    Ui::QChatRoomsClass ui;

    QWidget* m_container;
    QScrollArea* m_scrollArea;
};

