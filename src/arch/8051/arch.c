// SPDX-License-Identifier: GPL-3.0-only

#include <8051.h>

#include <arch/arch.h>
#include <arch/time.h>

void arch_init(void) {
    // Disable interrupts
    EA = 0;

    time_init();

    // Enable interrupts
    EA = 1;
}
