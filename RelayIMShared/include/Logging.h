#pragma once

#define LOG_NETWORK_EVENTS true
#define LOG_NETWORK_PACKET_TYPES true

#define LOG_UI true

#include <mutex>
#include <stdio.h>

// NOTE(Salads): LogArguments simply convert std::strings into c strings for proper printing
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

class Log
{
public:

    static Log* Get();
    static void Initialize(std::string filename);
    static void Destroy();

    Log(std::string logFilename);
    ~Log();

    Log(Log& log) = delete;

    // Simply prints a formatted string. Formatting is the same as printf.
    template <typename ... Args>
    void WriteLine(const char* fmt, Args const& ... args) noexcept
    {
        std::lock_guard lock(m_mutex);
        if(!m_initialized) { return; }

        std::string str = fmt;
        str += "\n";

        fprintf(m_file, str.c_str(), LogArgument(args) ...);
        fflush(m_file);
    };

    template <typename ... Args>
    void ConditionalWriteLine(bool condition, const char* fmt, Args const& ... args) noexcept
    {
        if(!condition) { return; }
        WriteLine(fmt, args ...);
    };

private:

    bool m_initialized = false;
    FILE* m_file = nullptr;
    std::mutex m_mutex;
};
