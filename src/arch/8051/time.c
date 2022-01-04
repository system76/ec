// SPDX-License-Identifier: GPL-3.0-only

// Use Timer 2 to keep track of global time.

#include <arch/time.h>

#include <8052.h>

// Pre-calculated value for a ~1ms interrupt using a 9.2 MHz crystal.
//   0x10000 - (INTERVAL * ((OSC_FREQ / 1000) / OSC_DIVISOR))
#define TIMER2_RELOAD 0xFD01

static volatile uint32_t time_overflows = 0;

void timer_2(void) __interrupt(5) {
    // Hardware automatically reloads the RCAP2 value.
    // Software must explicitly clear the interrupt.
    TF2 = 0;

    time_overflows++;
}

void time_init(void) __critical {
    time_overflows = 0;

    // Timer 2 defaults to 16-bit auto-reload

    // Set the initial values
    TH2 = TIMER2_RELOAD >> 8;
    TL2 = TIMER2_RELOAD & 0xFF;
    // Set the values to reload on overflow
    RCAP2H = TIMER2_RELOAD >> 8;
    RCAP2L = TIMER2_RELOAD & 0xFF;

    // Enable timer interrupts
    ET2 = 1;
    // Start the timer
    TR2 = 1;
}

uint32_t time_get(void) __critical {
    return time_overflows;
}
