#ifndef QCHATWIDGET_H
#define QCHATWIDGET_H

#include <QWidget>
#include <QListView>
#include <QLabel>

#include "PacketData.h"
#include "components/QChatInput/QChatInput.h"
#include "models/QModelManager/QModelManager.h"
#include "widgets/QChatView/QChatView.h"
#include "ui_QChatWidget.h"

class QChatWidget : public QWidget
{
    Q_OBJECT

public:
    QChatWidget(QModelManager* manager, QWidget *parent = nullptr);

    void SetRoomID(RoomID roomID);
    RoomID GetRoomID();

private:
    Ui::QChatWidgetClass ui;

    QModelManager* m_manager;

    QLabel* m_roomNameLabel;
    QChatView* m_chatListView;
    QChatInput* m_chatInput;

    RoomID m_currentRoomID;
};

#endif // QCHATWIDGET_H

