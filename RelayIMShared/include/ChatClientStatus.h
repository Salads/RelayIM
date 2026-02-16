#pragma once

enum ChatClientStatus
{
    ChatClientStatus_Fresh = 0,      // Client just connected, no other information received yet.
    ChatClientStatus_Connected = 1,  // Received registration information. (username)
};