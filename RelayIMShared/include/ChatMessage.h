#pragma once

#include <string>
#include "PeerID.h"

struct ChatMessage
{
    ChatMessage(PeerID peerID, std::string message);

    PeerID m_senderID;
    std::string m_message;
};