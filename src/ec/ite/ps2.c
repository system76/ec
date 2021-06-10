// SPDX-License-Identifier: GPL-3.0-only

#include <ec/ps2.h>

#define PS2(NUM) { \
    .control = &PSCTL ## NUM, \
    .interrupt = &PSINT ## NUM, \
    .status = &PSSTS ## NUM, \
    .data = &PSDAT ## NUM, \
}

struct Ps2 __code PS2_1 = PS2(1);
struct Ps2 __code PS2_2 = PS2(2);
struct Ps2 __code PS2_3 = PS2(3);

void ps2_reset(struct Ps2 * ps2) {
    // Reset interface to defaults
    *(ps2->control) = 1;
    // Clear status
    *(ps2->status) = *(ps2->status);
}
