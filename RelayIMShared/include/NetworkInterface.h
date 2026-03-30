#ifndef NETWORKINTERFACE_H
#define NETWORKINTERFACE_H

class NetworkInterface
{
public: 
    virtual bool initializeInterface() = 0;
    virtual void shutdownInterface() = 0;

protected:
    bool m_isInitialized = false;
};

#endif // NETWORKINTERFACE_H