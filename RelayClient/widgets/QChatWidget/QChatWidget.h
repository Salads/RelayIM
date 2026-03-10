#pragma once

#include <QWidget>
#include <QListView>
#include <QLabel>

#include "Types.h"
#include "components/QChatInput/QChatInput.h"
#include "models/QModelManager/QModelManager.h"
#include "ui_QChatWidget.h"

class QChatWidget : public QWidget
{
    Q_OBJECT

public:
    QChatWidget(QModelManager* manager, QWidget *parent = nullptr);

    void setRoom(RoomID roomID);

private:
    Ui::QChatWidgetClass ui;

    QModelManager* m_manager;

    QLabel* m_roomNameLabel;
    QListView* m_chatListView;
    QChatInput* m_chatInput;
};

