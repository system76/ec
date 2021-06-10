// SPDX-License-Identifier: GPL-3.0-only

#include <ec/ec.h>
#include <ec/gctrl.h>
#include <common/debug.h>
#include <common/macro.h>

void ec_init(void) {
    RSTS = 0x44;

    // Enable POST codes
    SPCTRL1 |= BIT(7) | BIT(6) | BIT(3);
}

void ec_read_post_codes(void) {
    while (P80H81HS & 1) {
        uint8_t p80h = P80HD;
        uint8_t p81h = P81HD;
        P80H81HS |= 1;

        DEBUG("POST %02X%02X\n", p81h, p80h);
    }
}
