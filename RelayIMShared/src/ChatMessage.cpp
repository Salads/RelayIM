#include "ChatMessage.h"

ChatMessage::ChatMessage(PeerID peerID, std::string message)
    :m_senderID(peerID), m_message(message)
{}
