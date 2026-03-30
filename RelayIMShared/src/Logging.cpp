#include "Logging.h"

Log* g_logInstance = nullptr;
Log* Log::get()
{
    if(!g_logInstance)
    {
        initialize("defaultlog.txt");
    }

    return g_logInstance;
}

void Log::initialize(std::string filename)
{
    if(!g_logInstance)
    {
        g_logInstance = new Log(filename);
    }
}

void Log::destroy()
{
    g_logInstance->~Log();
}

Log::Log(std::string logFilename)
{
    m_file = _fsopen(logFilename.c_str(), "w", _SH_DENYNO);
    m_initialized = m_file != nullptr;
}

Log::~Log()
{
    fclose(m_file);
}