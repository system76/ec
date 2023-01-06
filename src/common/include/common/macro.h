// SPDX-License-Identifier: GPL-3.0-only

#ifndef _COMMON_MACRO_H
#define _COMMON_MACRO_H

#define xconcat(a, b) concat(a, b)
#define concat(a, b) a##b

#define xstr(s) str(s)
#define str(s) #s

#define ARRAY_SIZE(X) (sizeof(X) / sizeof((X)[0]))

#define BIT(X) (1UL << (X))

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(val, min, max) (MIN(MAX(val, min), max))

#endif // _COMMON_MACRO_H
