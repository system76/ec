// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_ECPM_H
#define _EC_ECPM_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x1E01) CGCTRL1;
volatile uint8_t __xdata __at(0x1E02) CGCTRL2;
volatile uint8_t __xdata __at(0x1E03) PLLCTRL;
volatile uint8_t __xdata __at(0x1E04) AUTOCG;
volatile uint8_t __xdata __at(0x1E05) CGCTRL3;
volatile uint8_t __xdata __at(0x1E06) PLLFREQ;
volatile uint8_t __xdata __at(0x1E08) PLLCSS;
volatile uint8_t __xdata __at(0x1E09) CGCTRL4;

#endif // _EC_ECPM_H
