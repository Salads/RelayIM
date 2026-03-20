#pragma once

#include <QWidget>
#include <QScrollArea>
#include "ui_QChatView.h"

#include "ChatMessage.h"
#include "models/QModelManager/QModelManager.h"
#include "models/QChatModel/QChatModel.h"

class QChatView : public QScrollArea
{
    Q_OBJECT

public:
    QChatView(QModelManager* manager, QWidget *parent = nullptr);
    ~QChatView();

    void SetRoom(RoomID roomID);

private:
    Ui::QChatViewClass ui;

    QChatModel* m_model;

    QModelManager* m_manager;
};

