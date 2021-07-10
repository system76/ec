// SPDX-License-Identifier: GPL-3.0-only

#ifndef _ARCH_DELAY_H
#define _ARCH_DELAY_H

#include <board/cpu.h>
#include <util/delay.h>

#define delay_ms(ms) _delay_ms(ms)

#endif // _ARCH_DELAY_H
