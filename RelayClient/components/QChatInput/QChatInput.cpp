#include "QChatInput.h"
#include <qtextedit.h>
#include <qboxlayout.h>

QChatInput::QChatInput(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    setFixedHeight(30);

    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("QWidget { background-color: #f0f0f0; border: 1px solid #cccccc; border-radius: 5px; }");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(5, 2, 5, 2);

    QTextEdit* inputField = new QTextEdit(this);
    inputField->setPlaceholderText("Enter message...");
    inputField->setFixedHeight(30);
    inputField->setStyleSheet(
        "QTextEdit { "
        "border: none; "
        "color: #000000; "
        "} "
        "QTextEdit::placeholder { "
        "color: #999999; "
        "}"
    );
    mainLayout->addWidget(inputField);
}

QChatInput::~QChatInput()
{}

