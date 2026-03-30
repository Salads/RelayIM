#ifndef PEERID_H
#define PEERID_H

#include <cstdint>
#include <ostream>
#include <functional>

#define INVALID_PEER_ID -1

class PeerID
{
public:
    explicit PeerID(uint32_t id = INVALID_PEER_ID) : m_peerId(id) {}

    explicit operator uint32_t() const { return m_peerId; }

    PeerID& operator=(uint32_t rhs) { m_peerId = rhs; return *this; }

    bool operator==(const PeerID& rhs) const { return m_peerId == rhs.m_peerId; }
    bool operator==(const uint32_t& rhs) const { return m_peerId == rhs; }

    bool operator!=(const PeerID& rhs) const { return m_peerId != rhs.m_peerId; }
    bool operator!=(const uint32_t& rhs) const { return m_peerId != rhs; }

    friend std::ostream& operator<<(std::ostream& os, const PeerID& id)
    {
        return os << id.m_peerId;                                        
    }

private:
    uint32_t m_peerId;
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
