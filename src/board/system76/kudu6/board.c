// SPDX-License-Identifier: GPL-3.0-only

#include <board/battery.h>
#include <board/board.h>
#include <ec/ec.h>

void board_init(void) {
    // Make sure charger is in off state, also enables PSYS
    battery_charger_disable();
}

void board_event(void) {
    ec_read_post_codes();
}
