// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_DAC_H
#define _EC_DAC_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x1A01) DACPDREG;
volatile uint8_t __xdata __at(0x1A04) DACDAT2;
volatile uint8_t __xdata __at(0x1A05) DACDAT3;
volatile uint8_t __xdata __at(0x1A06) DACDAT4;
volatile uint8_t __xdata __at(0x1A07) DACDAT5;

#endif // _EC_DAC_H
