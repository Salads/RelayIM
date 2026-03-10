
#include <QBoxLayout>
#include <QFrame>

#include "widgets/QChatWidget/QChatWidget.h"

QChatWidget::QChatWidget(QModelManager *manager, QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    m_manager = manager;

    // Ensure this widget stretches to fill available space
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_roomNameLabel = new QLabel(this);
    m_roomNameLabel->setText("- No Chatroom -");

    QVBoxLayout *vLayoutMainContent = new QVBoxLayout(this);
    vLayoutMainContent->setContentsMargins(10, 0, 10, 0);

    m_chatListView = new QListView();
    m_chatListView->setSelectionMode(QListView::SelectionMode::NoSelection);

    m_chatInput = new QChatInput();

    vLayoutMainContent->addWidget(m_roomNameLabel, 0);
    vLayoutMainContent->addWidget(m_chatListView, 7);
    vLayoutMainContent->addWidget(m_chatInput, 2);
}

void QChatWidget::setRoom(RoomID roomID)
{
    std::shared_ptr<QChatRoomMessagesModel> model = m_manager->GetModelForRoom(roomID);
    Q_ASSERT(model);

    m_chatListView->setModel(m_manager->GetModelForRoom(roomID).get());
}