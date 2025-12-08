// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

#ifndef _EC_SCRATCH_H
#define _EC_SCRATCH_H

#include <stdint.h>

// SCAR0 is stored in processor cache, not in xram
volatile uint8_t __xdata __at(0x1040) SCAR0L;
volatile uint8_t __xdata __at(0x1041) SCAR0M;
volatile uint8_t __xdata __at(0x1042) SCAR0H;
#if CONFIG_EC_ITE_IT8587E
// SCAR1 is in xram at 0x800-0xC00
volatile uint8_t __xdata __at(0x1043) SCAR1L;
volatile uint8_t __xdata __at(0x1044) SCAR1M;
volatile uint8_t __xdata __at(0x1045) SCAR1H;
#endif

#if CONFIG_EC_ITE_IT8587E
#define SCARL SCAR1L
#define SCARM SCAR1M
#define SCARH SCAR1H
#else
#define SCARL SCAR0L
#define SCARM SCAR0M
#define SCARH SCAR0H
#endif

#endif // _EC_SCRATCH_H
