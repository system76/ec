// Uses timer 0 to keep track of global time

#include <8051.h>
#include <stdint.h>

#include <arch/time.h>

static volatile uint32_t time_overflows = 0;

void timer_0(void) __interrupt(1) {
    time_overflows++;
    
    // Restart timer
    TR0 = 0;
    TF0 = 0;
    TH0 = 0xFD;
    TL0 = 0x01;
    TR0 = 1;
}

void time_init(void) __critical {
    // Stop the timer
    TR0 = 0;
    TF0 = 0;

    time_overflows = 0;

    // Enable timer interrupts
    ET0 = 1;

    // Start timer in mode 1
    // (65536 - 64769) / (9.2 MHz / 12) = ~1 ms interval
    TMOD = (TMOD & 0xF0) | 0x01;
    TH0 = 0xFD;
    TL0 = 0x01;
    TR0 = 1;
}

uint32_t time_get(void) __critical {
    return time_overflows;
}
