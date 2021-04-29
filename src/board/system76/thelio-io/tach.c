// SPDX-License-Identifier: GPL-3.0-only

#include <board/tach.h>

struct Tach tach_new(uint64_t timeout) {
    struct Tach tach = {
        .timeout = timeout,
        .time = 0,
        .count = 0,
        .value = 0,
    };

    return tach;
}

uint16_t tach_step(struct Tach * tach, uint64_t time, uint16_t value) {
    tach->count += value;
    if (time >= tach->time) {
        tach->time = time + tach->timeout;
        tach->value = tach->count;
        tach->count = 0;
    }
    return tach->value;
}
