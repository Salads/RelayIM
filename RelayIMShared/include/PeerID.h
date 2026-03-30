#ifndef PEERID_H
#define PEERID_H

#include <cstdint>
#include <functional>
#include <ostream>

#define INVALID_PEER_ID -1

class PeerID
{
public:
    explicit PeerID(uint32_t id = INVALID_PEER_ID) : m_peerID(id) {}

    explicit operator uint32_t() const { return m_peerID; }

    PeerID& operator=(uint32_t rhs) { m_peerID = rhs; return *this; }

    bool operator==(const PeerID& rhs) const { return m_peerID == rhs.m_peerID; }
    bool operator==(const uint32_t& rhs) const { return m_peerID == rhs; }

    bool operator!=(const PeerID& rhs) const { return m_peerID != rhs.m_peerID; }
    bool operator!=(const uint32_t& rhs) const { return m_peerID != rhs; }

    friend std::ostream& operator<<(std::ostream& os, const PeerID& id)
    {
        return os << id.m_peerID;                                        
    }

private:
    uint32_t m_peerID;
};

namespace std
{
    template<>
    struct hash<PeerID>
    {
        size_t operator()(const PeerID& id) const
        {
            return std::hash<uint32_t>{}(static_cast<uint32_t>(id));
        }
    };
}

#endif // PEERID_H
