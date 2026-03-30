#ifndef QCHATWIDGET_H
#define QCHATWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QBoxLayout>

#include "components/QChatInput/QChatInput.h"
#include "models/QModelManager/QModelManager.h"
#include "models/QChatModel/QChatModel.h"
#include "widgets/QChatView/QChatView.h"

class QChatWidget : public QWidget
{
    Q_OBJECT

public:
    QChatWidget(QModelManager* manager, QWidget *parent = nullptr);

    void setRoomId(RoomID roomID);
    RoomID getRoomId();

private:
    QModelManager* m_manager;

    QLabel* m_roomNameLabel;
    QChatView* m_chatListView;
    QChatModel* m_chatModel;
    QChatInput* m_chatInput;

    RoomID m_currentRoomID;
};

#endif // QCHATWIDGET_H

