#pragma once

#include "NetworkTypes.h"

#define LOG_NETWORK_BYTESTREAM false

void LogDepth(uint32_t depth, const char* fmt, ...);
void LogDepthConditional(bool cond, uint32_t depth, const char* fmt, ...);