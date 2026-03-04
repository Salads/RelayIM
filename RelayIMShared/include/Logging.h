#pragma once

#include <mutex>

#include "NetworkTypes.h"

#define LOGGING_SPACES_PER_DEPTH 2

#define LOG_NETWORK_PACKETS false
#define LOG_NETWORK_PACKETS_DATA false
#define LOG_NETWORK_BYTESTREAM false

extern std::mutex g_loggingMutex;

template <typename T>
T LogArgument(T value) noexcept
{
    return value;
};

template <typename T>
T const* LogArgument(std::basic_string<T> const& value) noexcept
{
    return value.c_str();
};

template <typename ... Args>
void LogDepth(uint32_t depth, const char* fmt, Args const & ... args) noexcept
{
    std::lock_guard lock(g_loggingMutex);

    std::string str = fmt;

    if (depth > 0)
    {
        str = std::string(depth * LOGGING_SPACES_PER_DEPTH, ' ') + str;
    }

    printf(str.c_str(), LogArgument(args) ...);
};

template <typename ... Args>
void LogDepthConditional(bool cond, uint32_t depth, const char* fmt, Args const & ... args) noexcept
{
    if (!cond) { return; }

    LogDepth(depth, fmt, args ...);
}