// SPDX-License-Identifier: GPL-3.0-only

#include <common/macro.h>
#include <stdint.h>

#define INTC_BASE 0x1100

static volatile uint8_t __xdata __at(INTC_BASE + 0x10) IVCT;

static inline uint8_t interrupt_get_irq(void) {
    return IVCT - 0x10;
}

void external_1(void) __interrupt(2) {
}
