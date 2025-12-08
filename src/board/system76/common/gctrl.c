// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

#include <board/gctrl.h>

void gctrl_init(void) {
    // Set I2EC as R/W
    SPCTRL1 |= 0x03;
    // Set PNPCFG base address
    BADRSEL = 0;
}
