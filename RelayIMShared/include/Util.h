#ifndef UTIL_H
#define UTIL_H

#include <chrono>
#include <iostream>

#include "Logging.h"

#include <WinSock2.h>

void PrintWSAError(const char* message);

std::string GetLocalTimestamp();

#endif // UTIL_H