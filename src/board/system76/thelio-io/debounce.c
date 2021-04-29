// SPDX-License-Identifier: GPL-3.0-only

#include <board/debounce.h>

struct Debounce debounce_new(uint64_t timeout) {
    struct Debounce db = {
        .timeout = timeout,
        .time = 0,
        .value = 0
    };

    return db;
}

uint8_t debounce_step(struct Debounce * debounce, uint64_t time, uint8_t value) {
    // If the signal has been debounced for the timeout, stop debouncing
    if (time >= debounce->time) {
        debounce->time = 0;
    }
    // If the signal is being debounced, keep doing so, and override value
    else if (debounce->time) {
        value = debounce->value;
    }
    // If the signal changed, start debouncing
    else if (value != debounce->value) {
        debounce->time = time + debounce->timeout;
    }

    debounce->value = value;

    return debounce->value;
}
