#ifndef ROOMID_H
#define ROOMID_H

#include <cstdint>
#include <ostream>
#include <functional>

#define INVALID_ROOM_ID (uint32_t)-1

class RoomID
{
public:
    explicit RoomID(uint32_t id = INVALID_ROOM_ID) : m_roomId(id) {}

    explicit operator uint32_t() const { return m_roomId; }

    RoomID& operator=(uint32_t rhs) { m_roomId = rhs; return *this; }

    bool operator==(const RoomID& rhs) const { return m_roomId == rhs.m_roomId; }
    bool operator==(const uint32_t& rhs) const { return m_roomId == rhs; }

    bool operator!=(const RoomID& rhs) const { return m_roomId != rhs.m_roomId; }
    bool operator!=(const uint32_t& rhs) const { return m_roomId != rhs; }

    friend std::ostream& operator<<(std::ostream& os, const RoomID& id)
    {
        return os << id.m_roomId;
    }

private:
    uint32_t m_roomId;
};

namespace std
{
    template<>
    struct hash<RoomID>
    {
        size_t operator()(const RoomID& id) const
        {
            return std::hash<uint32_t>{}(static_cast<uint32_t>(id));
        }
    };
}

#endif // ROOMID_H
