// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2019 System76, Inc.

#ifndef _EC_GCTRL_H
#define _EC_GCTRL_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x2006) RSTS;
volatile uint8_t __xdata __at(0x200A) BADRSEL;
volatile uint8_t __xdata __at(0x200D) SPCTRL1;
#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
volatile uint8_t __xdata __at(0x2030) P80H81HS;
volatile uint8_t __xdata __at(0x2031) P80HD;
volatile uint8_t __xdata __at(0x2032) P81HD;
#endif

#endif // _EC_GCTRL_H
