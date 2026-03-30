#ifndef NETWORKINTERFACE_H
#define NETWORKINTERFACE_H


class NetworkInterface
{
public: 
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    bool GetIsInitialized() const { return m_isInitialized; }

protected:
    bool m_isInitialized = false;
};

#endif // NETWORKINTERFACE_H