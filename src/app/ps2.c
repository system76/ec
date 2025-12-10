// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2019 System76, Inc.

#include <app/ps2.h>

void ps2_init(void) {
    ps2_reset(&PS2_1);
    ps2_reset(&PS2_2);
    ps2_reset(&PS2_3);
}
