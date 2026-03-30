#ifndef LOGGING_H
#define LOGGING_H

// User-level network-data events
#define LOG_NETWORK_EVENTS true

// Packet receives and sends
#define LOG_NETWORK_PACKET_TYPES false

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

    static Log* get();
    static void initialize(std::string filename);
    static void destroy();

    Log(std::string logFilename);
    ~Log();

    Log(Log& log) = delete;

    // Simply prints a formatted string. Formatting is the same as printf.
    template <typename ... Args>
    void writeLine(const char* fmt, Args const& ... args) noexcept
    {
        std::lock_guard lock(m_mutex);
        if(!m_initialized) { return; }

        std::string str = fmt;
        str += "\n";

        fprintf(m_file, str.c_str(), LogArgument(args) ...);
        fflush(m_file);
    };

    template <typename ... Args>
    void conditionalWriteLine(bool condition, const char* fmt, Args const& ... args) noexcept
    {
        if(!condition) { return; }
        writeLine(fmt, args ...);
    };

private:

    bool m_initialized = false;
    FILE* m_file = nullptr;
    std::mutex m_mutex;
};

#endif // LOGGING_H
