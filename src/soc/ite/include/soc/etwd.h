// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

#ifndef _SOC_ETWD_H
#define _SOC_ETWD_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x1F01) ETWCFG;
volatile uint8_t __xdata __at(0x1F02) ET1PSR;
volatile uint8_t __xdata __at(0x1F04) ET1CNTLLR;
volatile uint8_t __xdata __at(0x1F06) EWDCNTLLR;
volatile uint8_t __xdata __at(0x1F07) EWDKEYR;
volatile uint8_t __xdata __at(0x1F09) EWDCNTLHR;

#endif // _SOC_ETWD_H
