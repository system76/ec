// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2019 System76, Inc.

// Uses timer 1 to implement delays

#include <8051.h>

#include <arch/delay.h>

void delay_ticks(uint16_t ticks) {
    uint16_t value = -ticks;

    // Stop the timer
    TR1 = 0;
    TF1 = 0;

    // Start timer in mode 1
    TMOD = (TMOD & 0x0F) | 0x10;
    TH1 = (uint8_t)(value >> 8);
    TL1 = (uint8_t)value;
    TR1 = 1;

    // Wait until complete
    while (TF1 == 0) {}
}

// This loops through delays of one ms in order to avoid overflow
void delay_ms(uint8_t ms) {
    for (uint8_t i = ms; i != 0; i--) {
        delay_us(1000);
    }
}
