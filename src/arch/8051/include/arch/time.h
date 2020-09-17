// SPDX-License-Identifier: GPL-3.0-only

#ifndef _ARCH_TIME_H
#define _ARCH_TIME_H

#include <stdint.h>

void time_init(void);
uint32_t time_get(void);

#endif // _ARCH_TIME_H
