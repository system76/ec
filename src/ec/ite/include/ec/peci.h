// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_PECI_H
#define _EC_PECI_H

#include <stdint.h>

// Host Status Register
static volatile uint8_t __xdata __at(0x3000) HOSTAR;
// Host Control Register
static volatile uint8_t __xdata __at(0x3001) HOCTLR;
// Host Command (Write 1) Register
static volatile uint8_t __xdata __at(0x3002) HOCMDR;
// Host Target Address Register
static volatile uint8_t __xdata __at(0x3003) HOTRADDR;
// Host Write Length Register
static volatile uint8_t __xdata __at(0x3004) HOWRLR;
// Host Read Length Register
static volatile uint8_t __xdata __at(0x3005) HORDLR;
// Host Write Data (2-16) Register (FIFO)
static volatile uint8_t __xdata __at(0x3006) HOWRDR;
// Host Read Data (1-16) Register (FIFO)
static volatile uint8_t __xdata __at(0x3007) HORDDR;
// Host Control 2 Register
static volatile uint8_t __xdata __at(0x3008) HOCTL2R;
// Received Write FCS Value
static volatile uint8_t __xdata __at(0x3009) RWFCSV;
// Received Read FCS Value
static volatile uint8_t __xdata __at(0x300A) RRFCSV;
// Write FCS Value
static volatile uint8_t __xdata __at(0x300B) WFCSV;
// Read FCS Value
static volatile uint8_t __xdata __at(0x300C) RFCSV;
// Assured Write FCS Value
static volatile uint8_t __xdata __at(0x300D) AWFCSV;
// Pad Control Register
static volatile uint8_t __xdata __at(0x300E) PADCTLR;

#endif // _EC_PECI_H
