// SPDX-License-Identifier: GPL-3.0-only

// Uses timer 0 to keep track of global time

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include <arch/time.h>
#include <board/cpu.h>

// We will use CS02 to divide clock by 256
#define TIME_TCNT_SCALE 256ULL
// Dividing 16 MHz by 256 = 62.5 KHz
#define TIME_TCNT_FREQUENCY (F_CPU/TIME_TCNT_SCALE)
// 62.5 KHz is 16 us resolution
#define TIME_TCNT_INTERVAL (1000000ULL/TIME_TCNT_FREQUENCY)
// 250 is an optimal factor, it is the square root of 625,000
#define TIME_OVERFLOW_SCALE 250ULL
// Dividing 62.5 KHz by 250 = 250 Hz
#define TIME_OVERFLOW_FREQUENCY (TIME_TCNT_FREQUENCY/TIME_OVERFLOW_SCALE)
// 250 Hz is 4 ms resolution
#define TIME_OVERFLOW_INTERVAL (1000000ULL/TIME_OVERFLOW_FREQUENCY)

// At 4 milliseconds per tick, an overflow of this variable would happen after 2,338,218,130 years
static volatile uint64_t time_overflows = 0;

static volatile uint8_t * time_tcnt = &TCNT0;

ISR(TIMER0_COMPA_vect) {
    time_overflows++;
}

void time_init(void) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        // Disable interrupts
        TIMSK0 = 0;
        // Clear COM set WGM mode[0:1]
        TCCR0A = _BV(WGM01);
        // Clear WGM mode[2] and set CS to 256
        TCCR0B = _BV(CS02);
        // Reset timer to 0
        TCNT0 = 0;
        // Set overflow value to that calculated above
        OCR0A = TIME_OVERFLOW_SCALE;
        // Clear current overflows
        time_overflows = 0;
        // Enable interrupts
        TIMSK0 = _BV(OCF0A);
    }
}

uint32_t time_get(void) {
    uint64_t overflows;
    uint8_t tcnt_0;
    uint8_t tcnt_1;

    do {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            tcnt_0 = *(time_tcnt);
        }
        // If an interrupt happens here, tcnt will have changed and so will overflows
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            overflows = time_overflows;
            tcnt_1 = *(time_tcnt);
        }
    } while (tcnt_0 != tcnt_1);

    return (uint32_t)(overflows * TIME_OVERFLOW_INTERVAL + ((uint64_t)tcnt_0) * TIME_TCNT_INTERVAL);
}
