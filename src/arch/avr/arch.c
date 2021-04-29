// SPDX-License-Identifier: GPL-3.0-only

#include <avr/interrupt.h>

#include <arch/arch.h>
#include <arch/time.h>

void arch_init(void) {
    // Disable interrupts
    cli();

    time_init();

    // Enable interrupts
    sei();
}
