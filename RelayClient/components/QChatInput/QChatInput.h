#ifndef QCHATINPUT_H
#define QCHATINPUT_H

#include <QTextEdit>
#include <QKeyEvent>

#include "models/QModelManager/QModelManager.h"

class QChatInput : public QTextEdit
{
    Q_OBJECT

public:
    QChatInput(QModelManager* manager, QWidget* parent = nullptr);

    void keyPressEvent(QKeyEvent* event) override;
    void SetRoomID(RoomID roomID);

private:
    QModelManager* m_manager;
    RoomID m_roomID;
};

#endif // QCHATINPUT_H

