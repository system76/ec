// SPDX-License-Identifier: GPL-3.0-only

#include <ec/ec.h>
#include <ec/gctrl.h>
#include <common/debug.h>
#include <common/macro.h>

static enum EcResetSource ec_lsr = RESET_SOURCE_NORMAL;

enum EcResetSource ec_reset_source(void) {
    return ec_lsr;
}

static void ec_check_reset_source(void) {
    // LSR field in RSTS only reports WDT reset.
    uint8_t rsts = RSTS & 0b11;
    // Only write bit 0 to clear field.
    RSTS |= BIT(0);

    switch (rsts) {
    case 0:
    case 1:
        // VSTBY or WRST#
        ec_lsr = RESET_SOURCE_NORMAL;
        break;
    case 2:
    case 3:
        // Internal/External WDT
        ec_lsr = RESET_SOURCE_WDT;
        break;
    }

    // SPCTRL4 reports more reset sources.
    uint8_t lsr = SPCTRL4;
    // All bits are write-clear.
    SPCTRL4 = 0xFF;

    if (lsr & BIT(1)) {
        // PWRSW WDT
        ec_lsr = RESET_SOURCE_PWRSW_TIMEOUT;
    }
}

void ec_init(void) {
    ec_check_reset_source();

#if CONFIG_EC_ITE_IT8587E
    RSTS = (0b10U << 6) | BIT(2);
#else
    RSTS = (0b01U << 6) | BIT(2);

    // Enable POST codes
    SPCTRL1 |= BIT(7) | BIT(6) | BIT(3);
#endif
}

void ec_read_post_codes(void) {
#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
    while (P80H81HS & 1) {
        uint8_t p80h = P80HD;
        uint8_t p81h = P81HD;
        P80H81HS |= 1;

        DEBUG("POST %02X%02X\n", p81h, p80h);
    }
#endif
}
