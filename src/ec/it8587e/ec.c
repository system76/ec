// SPDX-License-Identifier: GPL-3.0-only

#include <ec/ec.h>
#include <ec/gctrl.h>

void ec_init(void) {
    RSTS = 0x84;
}
