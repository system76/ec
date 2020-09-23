// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_SCRATCH_H
#define _EC_SCRATCH_H

#include <stdint.h>

// SCAR1 is in xram at 0x800-0xC00
volatile uint8_t __xdata __at(0x1043) SCAR1L;
volatile uint8_t __xdata __at(0x1044) SCAR1M;
volatile uint8_t __xdata __at(0x1045) SCAR1H;
#define SCARL SCAR1L
#define SCARM SCAR1M
#define SCARH SCAR1H

#endif // _EC_SCRATCH_H
