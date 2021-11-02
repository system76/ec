// SPDX-License-Identifier: GPL-3.0-only

#include <ec/bram.h>
#include <ec/ec.h>
#include <ec/gctrl.h>
#include <common/debug.h>
#include <common/macro.h>

void ec_init(void) {
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

#ifdef POST_CODE_RING_BUFFER
        DEBUG("POST %02X%02X:", p81h, p80h);

        // Also dump ring-buffer
        for (uint8_t i = 0x80; i <= 0xBF; i += 2) {
            DEBUG(" %02X%02X", BRAM[i + 1], BRAM[i]);
        }
        DEBUG("\n");
#else // POST_CODE_RING_BUFFER
        DEBUG("POST %02X%02X\n", p81h, p80h);
#endif // POST_CODE_RING_BUFFER
    }
#endif
}
