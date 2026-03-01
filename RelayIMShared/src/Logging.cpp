#include "Logging.h"

#include <cstdarg>
#include <string>
#include <thread>
#include <mutex>

std::mutex g_loggingMutex;