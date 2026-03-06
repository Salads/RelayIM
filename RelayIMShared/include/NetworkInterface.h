#pragma once

#include "NetworkTypes.h"

class NetworkInterface
{
public: 
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    bool GetIsInitialized() const { return m_isInitialized; }

protected:
    bool m_isInitialized = false;
};