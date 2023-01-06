// SPDX-License-Identifier: GPL-3.0-only

#ifndef _COMMON_DEBUG_H
#define _COMMON_DEBUG_H

#include <stdio.h>

#define LEVEL_TRACE 5
#define LEVEL_DEBUG 4
#define LEVEL_INFO 3
#define LEVEL_WARN 2
#define LEVEL_ERROR 1
#define LEVEL_NONE 0

// This is the user-configurable log level
#ifndef LEVEL
#define LEVEL LEVEL_INFO
#endif

#if LEVEL >= LEVEL_TRACE
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if LEVEL >= LEVEL_DEBUG
#define DEBUG(...) printf(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#if LEVEL >= LEVEL_INFO
#define INFO(...) printf(__VA_ARGS__)
#else
#define INFO(...)
#endif

#if LEVEL >= LEVEL_WARN
#define WARN(...) printf(__VA_ARGS__)
#else
#define WARN(...)
#endif

#if LEVEL >= LEVEL_ERROR
#define ERROR(...) printf(__VA_ARGS__)
#else
#define ERROR(...)
#endif

#endif // _COMMON_DEBUG_H
