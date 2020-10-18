// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_GCTRL_H
#define _EC_GCTRL_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x2006) RSTS;
volatile uint8_t __xdata __at(0x200A) BADRSEL;
volatile uint8_t __xdata __at(0x200D) SPCTRL1;
volatile uint8_t __xdata __at(0x2016) SPCTRL3;
volatile uint8_t __xdata __at(0x2030) P80H81HS;
volatile uint8_t __xdata __at(0x2031) P80HD;
volatile uint8_t __xdata __at(0x2032) P81HD;

#endif // _EC_GCTRL_H
