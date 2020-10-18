// SPDX-License-Identifier: GPL-3.0-only

#ifndef _COMMON_MACRO_H
#define _COMMON_MACRO_H

#define xstr(s) str(s)
#define str(s) #s

#define ARRAY_SIZE(X) (sizeof(X) / sizeof((X)[0]))

#define BIT(X) (1UL << (X))

#endif // _COMMON_MACRO_H
