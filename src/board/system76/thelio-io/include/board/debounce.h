// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_DEBOUNCE_H
#define _BOARD_DEBOUNCE_H

#include <stdint.h>

struct Debounce {
    // Total time to debounce
    uint64_t timeout;
    // When debouncing will end
    uint64_t time;
    // Last value of the signal
    uint8_t value;
};

// Create debounce object with a timeout in microseconds
struct Debounce debounce_new(uint64_t timeout);

// Debounce, returning the current state of the signal with bounces removed
uint8_t debounce_step(struct Debounce * debounce, uint64_t time, uint8_t value);

#endif // _BOARD_DEBOUNCE_H
