// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_PS2_H
#define _EC_PS2_H

#include <stdint.h>

#define PSSTS_TIMEOUT_ERR BIT(6)
#define PSSTS_FRAME_ERR BIT(5)
#define PSSTS_PARITY_ERR BIT(4)
#define PSSTS_ALL_ERR (PSSTS_TIMEOUT_ERR | PSSTS_FRAME_ERR | PSSTS_PARITY_ERR)
#define PSSTS_DONE BIT(3)

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
