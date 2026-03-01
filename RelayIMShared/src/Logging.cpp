#include "Logging.h"

#include <cstdarg>
#include <string>
#include <thread>
#include <mutex>

#define LOGGING_SPACES_PER_DEPTH 2

static std::mutex g_loggingMutex;

void LogDepth(uint32_t depth, const char* fmt, ...)
{
    std::lock_guard lock(g_loggingMutex);

    va_list args;
    va_start(args, fmt);

    std::string str = fmt;

    if (depth > 0)
    {
        str = std::string(depth * LOGGING_SPACES_PER_DEPTH, ' ') + str;
    }

    vprintf(str.c_str(), args);

    va_end(args);
}

void LogDepthConditional(bool cond, uint32_t depth, const char* fmt, ...)
{
    if (!cond) { return; }

    va_list args;
    va_start(args, fmt);

    LogDepth(depth, fmt, args);

    va_end(args);
}