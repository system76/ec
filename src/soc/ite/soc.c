// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

#include <soc/soc.h>
#include <soc/gctrl.h>
#include <common/debug.h>
#include <common/macro.h>

void soc_init(void) {
#if CONFIG_EC_ITE_IT8587E
    RSTS = (0b10U << 6) | BIT(2);
#else
    RSTS = (0b01U << 6) | BIT(2);

    // Enable POST codes
    SPCTRL1 |= BIT(7) | BIT(6) | BIT(3);
#endif
}

void soc_read_post_codes(void) {
#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
    while (P80H81HS & 1) {
        uint8_t p80h = P80HD;
        uint8_t p81h = P81HD;
        P80H81HS |= 1;

        DEBUG("POST %02X%02X\n", p81h, p80h);
    }
#endif
}
