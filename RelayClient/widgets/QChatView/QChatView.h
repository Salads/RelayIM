#pragma once

#include <QWidget>
#include <QScrollArea>
#include <QStyle>
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

    int GetViewportWidth();

protected:
    void resizeEvent(QResizeEvent* event);

private:
    Ui::QChatViewClass ui;

    QChatModel* m_model;

    QModelManager* m_manager;
};

