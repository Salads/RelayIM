#include "QChatInput.h"
#include <qtextedit.h>
#include <qboxlayout.h>

QChatInput::QChatInput(QModelManager *manager, QWidget* parent)
    : QTextEdit(parent)
{
    m_manager = manager;
}

void QChatInput::keyPressEvent(QKeyEvent* event)
{
    if(m_roomId == INVALID_ROOM_ID) { return; }

    if(event->key() == Qt::Key_Return)
    {
        if(event->modifiers() & Qt::KeyboardModifier::ShiftModifier)
        {
            setPlainText(toPlainText() + "\n");
            moveCursor(QTextCursor::MoveOperation::End);
        }
        else
        {
            QString currentText = toPlainText();
            QString currentTextTrimmed = currentText.trimmed();
            if(!currentTextTrimmed.isEmpty())
            {
                m_manager->getClient()->sendMessageToRoom(m_roomId, currentText.toStdString());
                clear();
            }
        }
    }
    else
    {
        QTextEdit::keyPressEvent(event);
    }
}

void QChatInput::setRoomId(RoomID roomID)
{
    m_roomId = roomID;
}
