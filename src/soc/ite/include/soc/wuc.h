// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2022 System76, Inc.

// Wake-Up Control (WUC)

// WUEMR: Wake-Up Edge Mode Register
// WUESR: Wake-Up Edge Sense Register
// WUENR: Wake-Up Enable Register

#ifndef _SOC_WUC_H
#define _SOC_WUC_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x1B00) WUEMR1;
volatile uint8_t __xdata __at(0x1B04) WUESR1;
volatile uint8_t __xdata __at(0x1B08) WUENR1;

volatile uint8_t __xdata __at(0x1B01) WUEMR2;
volatile uint8_t __xdata __at(0x1B05) WUESR2;

volatile uint8_t __xdata __at(0x1B02) WUEMR3;
volatile uint8_t __xdata __at(0x1B06) WUESR3;
volatile uint8_t __xdata __at(0x1B0A) WUENR3;

volatile uint8_t __xdata __at(0x1B03) WUEMR4;
volatile uint8_t __xdata __at(0x1B07) WUESR4;
volatile uint8_t __xdata __at(0x1B0B) WUENR4;

// Group 5 not defined

volatile uint8_t __xdata __at(0x1B10) WUEMR6;
volatile uint8_t __xdata __at(0x1B11) WUESR6;

volatile uint8_t __xdata __at(0x1B14) WUEMR7;
volatile uint8_t __xdata __at(0x1B15) WUESR7;

volatile uint8_t __xdata __at(0x1B18) WUEMR8;
volatile uint8_t __xdata __at(0x1B19) WUESR8;

volatile uint8_t __xdata __at(0x1B1C) WUEMR9;
volatile uint8_t __xdata __at(0x1B1D) WUESR9;

volatile uint8_t __xdata __at(0x1B20) WUEMR10;
volatile uint8_t __xdata __at(0x1B21) WUESR10;

volatile uint8_t __xdata __at(0x1B24) WUEMR11;
volatile uint8_t __xdata __at(0x1B25) WUESR11;

volatile uint8_t __xdata __at(0x1B28) WUEMR12;
volatile uint8_t __xdata __at(0x1B29) WUESR12;

volatile uint8_t __xdata __at(0x1B2C) WUEMR13;
volatile uint8_t __xdata __at(0x1B2D) WUESR13;

volatile uint8_t __xdata __at(0x1B30) WUEMR14;
volatile uint8_t __xdata __at(0x1B31) WUESR14;

enum WucEdgeMode {
    EDGE_RISING,
    EDGE_FALLING,
    EDGE_BOTH,
};

void wuc_enable(uint8_t nr, enum WucEdgeMode detect);
void wuc_disable(uint8_t nr);
void wuc_clear(uint8_t nr);

#endif // _SOC_WUC_H
