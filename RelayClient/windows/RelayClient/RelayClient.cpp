#include "RelayClient.h"
#include <qboxlayout.h>
#include <qpushbutton.h>
#include "widgets/QChatWidget/QChatWidget.h"
#include "widgets/QChatRooms/QChatRooms.h"

RelayClient::RelayClient(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this); // Generated UI from visual editor

    QHBoxLayout* hLayoutMainContent = new QHBoxLayout(this->centralWidget()); // Main Content Layout (ChatRooms, Chat Area)
    hLayoutMainContent->setContentsMargins(10, 10, 10, 10);

    QChatRooms* chatRooms = new QChatRooms();
    hLayoutMainContent->addWidget(chatRooms, 2);

    QChatWidget* chatWidget = new QChatWidget();
    hLayoutMainContent->addWidget(chatWidget, 8);

    connect(chatRooms, &QChatRooms::roomSelected, this, [chatWidget](QChatRoom* button) {
        chatWidget->setRoom(0, "Test");
    });

    this->setMinimumSize(550, 350);
}

RelayClient::~RelayClient()
{}

