#include "RelayClient.h"
#include <qboxlayout.h>
#include <qpushbutton.h>
#include "src/components/QChatWidget.h"

RelayClient::RelayClient(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this); // Generated UI from visual editor

    QHBoxLayout* hLayoutMainContent = new QHBoxLayout(); // Main Content Layout (ChatRooms, Chat Area)
    m_chatRoomsLayout = new QVBoxLayout();

    this->centralWidget()->setLayout(hLayoutMainContent);
    hLayoutMainContent->addLayout(m_chatRoomsLayout, 2.5);

    QChatWidget* chatWidget = new QChatWidget(this);
    hLayoutMainContent->addWidget(chatWidget, 7.5);

    QPushButton* button1 = new QPushButton("Button 1");
    m_chatRoomsLayout->addWidget(button1);
}

RelayClient::~RelayClient()
{}

