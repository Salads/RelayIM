#ifndef UTIL_H
#define UTIL_H

#include <chrono>
#include <iostream>

#include "Logging.h"

#include <WinSock2.h>

void printWSAError(const char* message);

std::string getLocalTimestamp();

#endif // UTIL_H