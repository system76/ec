// SPDX-License-Identifier: GPL-3.0-only

#include <board/gctrl.h>

void gctrl_init(void) {
    // Set I2EC as R/W
    SPCTRL1 |= 0x03;
    // Set PNPCFG base address
    BADRSEL = 0;
}
