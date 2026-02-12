#include "QChatHistory.h"

QChatHistory::QChatHistory(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    // Create main layout for this widget to hold the scroll area
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Create scroll area and add it to main layout so it fills the parent
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(m_scrollArea);

    // Create inner container for messages
    m_messageContainer = new QWidget();

    m_layout = new QVBoxLayout(m_messageContainer);
    m_layout->setSpacing(10);
    m_layout->setContentsMargins(4, 4, 4, 4);
    m_layout->setDirection(QBoxLayout::Direction::TopToBottom); // Ordering of elements
    // Add stretch at the beginning to push messages to the bottom. This keeps
    // messages anchored to the bottom even when the scroll area is larger than content.
    m_layout->insertStretch(0, 1);

    m_scrollArea->setWidget(m_messageContainer);

    // TEMP(Salads): Just to fill stuff in
    for (int i = 0; i < 10; i++)
    {
        addMessage("Example Message " + std::to_string(i));
    }
}

QChatHistory::~QChatHistory()
{}

void QChatHistory::addMessage(std::string message)
{
    // Keep messages in chronological order in m_rawMessages (oldest..newest)
    m_rawMessages.push_back(message);
    updateMessages();
}

void QChatHistory::updateMessages()
{
    // TODO(Salads): Calculate the number of messages that can be displayed based on the height of the widget and the height of each message.
    const int maxMessages = 10;

    size_t numMessages = m_messages.size();
    // newest message is at the back of m_rawMessages
    const std::string &latest = m_rawMessages.back();

    if (numMessages < maxMessages) // add a new widget and append it to the layout (newest at bottom)
    {
        QMessage *newMessage = new QMessage(this);
        newMessage->setMessage(latest);
        m_messages.push_back(newMessage);

        // Insert after the stretch (at position numMessages + 1)
        m_layout->insertWidget(numMessages + 1, newMessage);
    }
    else // reuse the oldest widget (front) and move it to the back so newest stays at bottom
    {
        QMessage *oldestMessage = m_messages.front();
        oldestMessage->setMessage(latest);
        m_layout->removeWidget(oldestMessage);
        
        // Insert at the position after the stretch
        m_layout->insertWidget(numMessages, oldestMessage); 
        m_messages.pop_front();
        m_messages.push_back(oldestMessage);
    }

    // Defer scrolling to the bottom until after the layout has processed the new widget.
    // Using QTimer with 0ms delay schedules it for the next event loop iteration.
    QTimer::singleShot(0, this, [this]() {
        m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->maximum());
    });
}