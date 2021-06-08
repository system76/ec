// SPDX-License-Identifier: GPL-3.0-only

#include <ec/intc.h>
#include <ec/gctrl.h>
#include <common/debug.h>

// Port 0x80/0x81 interrupt
void isr_post_code(void) {
    uint8_t p80h = P80HD;
    uint8_t p81h = P81HD;
    P80H81HS |= 1;

    DEBUG("POST %02X%02X\n", p81h, p80h);
}

