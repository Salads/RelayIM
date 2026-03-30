#ifndef UTIL_H
#define UTIL_H

#include <chrono>

#include "Logging.h"

void PrintWSAError(const char* message);

std::string GetLocalTimestamp();

#endif // UTIL_H