// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_SCRATCH_H
#define _EC_SCRATCH_H

#include <stdint.h>

// SCAR0 is stored in processor cache, not in xram
volatile uint8_t __xdata __at(0x1040) SCAR0L;
volatile uint8_t __xdata __at(0x1041) SCAR0M;
volatile uint8_t __xdata __at(0x1042) SCAR0H;
#define SCARL SCAR0L
#define SCARM SCAR0M
#define SCARH SCAR0H

#endif // _EC_SCRATCH_H
