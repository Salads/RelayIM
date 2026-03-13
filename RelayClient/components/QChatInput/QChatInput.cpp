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
    if(m_roomID == INVALID_ROOM_ID) { return; }

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
                m_manager->GetClient()->SendMessageToRoom(m_roomID, currentText.toStdString());
                clear();
            }
        }
    }
    else
    {
        QTextEdit::keyPressEvent(event);
    }
}

void QChatInput::setRoom(RoomID roomID)
{
    m_roomID = roomID;
}
