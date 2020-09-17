// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_PS2_H
#define _EC_PS2_H

#include <stdint.h>

struct Ps2 {
    volatile uint8_t * control;
    volatile uint8_t * interrupt;
    volatile uint8_t * status;
    volatile uint8_t * data;
};

extern struct Ps2 __code PS2_1;
extern struct Ps2 __code PS2_2;
extern struct Ps2 __code PS2_3;

void ps2_reset(struct Ps2 * ps2);
int ps2_read(struct Ps2 * ps2, uint8_t * data, int length);
int ps2_write(struct Ps2 * ps2, uint8_t * data, int length);

volatile uint8_t __xdata __at(0x1700) PSCTL1;
volatile uint8_t __xdata __at(0x1701) PSCTL2;
volatile uint8_t __xdata __at(0x1702) PSCTL3;

volatile uint8_t __xdata __at(0x1704) PSINT1;
volatile uint8_t __xdata __at(0x1705) PSINT2;
volatile uint8_t __xdata __at(0x1706) PSINT3;

volatile uint8_t __xdata __at(0x1708) PSSTS1;
volatile uint8_t __xdata __at(0x1709) PSSTS2;
volatile uint8_t __xdata __at(0x170A) PSSTS3;

volatile uint8_t __xdata __at(0x170C) PSDAT1;
volatile uint8_t __xdata __at(0x170D) PSDAT2;
volatile uint8_t __xdata __at(0x170E) PSDAT3;

#endif // _EC_PS2_H
