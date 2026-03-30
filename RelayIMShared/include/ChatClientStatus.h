#ifndef CHATCLIENTSTATUS_H
#define CHATCLIENTSTATUS_H

enum ChatClientStatus
{
    ChatClientStatus_Fresh = 0,      // Client just connected, no other information received yet.
    ChatClientStatus_Connected = 1,  // Received registration information. (username)
};

#endif // CHATCLIENTSTATUS_H