// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_PECI_H
#define _EC_PECI_H

#include <stdint.h>

static volatile uint8_t __xdata __at(0x3000) HOSTAR;
static volatile uint8_t __xdata __at(0x3001) HOCTLR;
static volatile uint8_t __xdata __at(0x3002) HOCMDR;
static volatile uint8_t __xdata __at(0x3003) HOTRADDR;
static volatile uint8_t __xdata __at(0x3004) HOWRLR;
static volatile uint8_t __xdata __at(0x3005) HORDLR;
static volatile uint8_t __xdata __at(0x3006) HOWRDR;
static volatile uint8_t __xdata __at(0x3007) HORDDR;
static volatile uint8_t __xdata __at(0x3008) HOCTL2R;
static volatile uint8_t __xdata __at(0x3009) RWFCSV;
static volatile uint8_t __xdata __at(0x300E) PADCTLR;

#endif // _EC_PECI_H
