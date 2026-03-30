#ifndef QCHATVIEW_H
#define QCHATVIEW_H

#include <QWidget>
#include <QScrollArea>
#include <QStyle>
#include <QResizeEvent>

#include "ChatMessage.h"
#include "models/QModelManager/QModelManager.h"

class QChatView : public QScrollArea
{
    Q_OBJECT

public:
    QChatView(QModelManager* manager, QWidget *parent = nullptr);
    ~QChatView();

    int getViewportWidth();

signals:
    void onResize(QSize oldSize, QSize newSize);

protected:
    void resizeEvent(QResizeEvent* event);

private:
    QModelManager* m_manager;
};

#endif // QCHATVIEW_H

