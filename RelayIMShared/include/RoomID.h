#pragma once

#include <cstdint>
#include <ostream>
#include <functional>

#define INVALID_ROOM_ID (uint32_t)-1

class RoomID
{
public:
    explicit RoomID(uint32_t id = INVALID_ROOM_ID) : m_roomID(id) {}

    explicit operator uint32_t() const { return m_roomID; }

    RoomID& operator=(uint32_t rhs) { m_roomID = rhs; return *this; }

    bool operator==(const RoomID& rhs) const { return m_roomID == rhs.m_roomID; }
    bool operator==(const uint32_t& rhs) const { return m_roomID == rhs; }

    bool operator!=(const RoomID& rhs) const { return m_roomID != rhs.m_roomID; }
    bool operator!=(const uint32_t& rhs) const { return m_roomID != rhs; }

    friend std::ostream& operator<<(std::ostream& os, const RoomID& id)
    {
        return os << id.m_roomID;
    }

private:
    uint32_t m_roomID;
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
