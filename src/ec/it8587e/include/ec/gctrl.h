// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_GCTRL_H
#define _EC_GCTRL_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x2006) RSTS;
volatile uint8_t __xdata __at(0x200A) BADRSEL;
volatile uint8_t __xdata __at(0x200D) SPCTRL1;

#endif // _EC_GCTRL_H
