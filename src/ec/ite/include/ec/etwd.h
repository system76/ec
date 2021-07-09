// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_ECWD_H
#define _EC_ECWD_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x1F01) ETWCFG;
volatile uint8_t __xdata __at(0x1F02) ET1PSR;
volatile uint8_t __xdata __at(0x1F04) ET1CNTLLR;
volatile uint8_t __xdata __at(0x1F06) EWDCNTLLR;
volatile uint8_t __xdata __at(0x1F07) EWDKEYR;
volatile uint8_t __xdata __at(0x1F09) EWDCNTLHR;

#endif // _EC_ECWD_H
