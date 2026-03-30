#include "ChatMessage.h"

ChatMessage::ChatMessage(PeerID peerID, std::string message)
    :m_senderId(peerID), m_message(message)
{}
