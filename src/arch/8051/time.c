// SPDX-License-Identifier: GPL-3.0-only

// Uses timer 0 to keep track of global time

#include <8051.h>
#include <arch/time.h>

#define OSC_DIVISOR 12
#define TICK_INTERVAL_MS 1
// Value to reload into the timer when the overflow interrupt is triggered.
#define TIMER_RELOAD (0xFFFF - ((TICK_INTERVAL_MS) * ((CONFIG_CLOCK_FREQ_KHZ) / OSC_DIVISOR)))

static volatile uint32_t time_overflows = 0;

void timer_0(void) __interrupt(1) {
    // Hardware automatically clears the the interrupt

    // Stop timer
    TR0 = 0;

    time_overflows++;

    // Reload the values
    TH0 = TIMER_RELOAD >> 8;
    TL0 = TIMER_RELOAD & 0xFF;

    // Restart the timer
    TR0 = 1;
}

/**
 * Set up Timer 0 as the system tick.
 */
void time_init(void) __critical {
    // Stop the timer
    TR0 = 0;
    TF0 = 0;

    time_overflows = 0;

    // Enable the interrupt
    ET0 = 1;

    // Set the timer to mode 1 (16-bit timer)
    TMOD = (TMOD & 0xF0) | 0x01;

    // Set the initial values
    TH0 = TIMER_RELOAD >> 8;
    TL0 = TIMER_RELOAD & 0xFF;

    // Start the timer
    TR0 = 1;
}

uint32_t time_get(void) __critical {
    return time_overflows;
}
