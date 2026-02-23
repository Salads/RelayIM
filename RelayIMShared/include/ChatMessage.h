#pragma once

#include <string>
#include "NetworkTypes.h"

struct ChatMessage
{
    ChatMessage(PeerID peerID, std::string message);

    PeerID m_senderID;
    std::string m_message;
};