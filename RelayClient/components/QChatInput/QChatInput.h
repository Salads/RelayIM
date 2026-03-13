#pragma once

#include <QTextEdit>
#include <QKeyEvent>

#include "models/QModelManager/QModelManager.h"

#include "ui_QChatInput.h"

class QChatInput : public QTextEdit
{
    Q_OBJECT

public:
    QChatInput(QModelManager* manager, QWidget* parent = nullptr);

    void keyPressEvent(QKeyEvent* event) override;
    void setRoom(RoomID roomID);

private:
    Ui::QChatInputClass ui;

    QModelManager* m_manager;

    RoomID m_roomID = INVALID_ROOM_ID;
};

