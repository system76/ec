// SPDX-License-Identifier: GPL-3.0-only

#include <board/interrupts.h>
#include <board/gpio.h>
#include <board/power.h>
#include <common/macro.h>
#include <ec/intc.h>
#include <ec/wuc.h>

void interrupts_init(void) {
    GCR14 = BIT(3);
    GCR16 = 2; // 64ms debounce for PWR_SW#

    // INT1: PWR_SW#
    WUEMR2 = BIT(0); // Falling-edge
    WUESR2 = BIT(0);
    ISR0 = BIT(1);
    IER0 = BIT(1);
}

void external_1(void) __interrupt(2) {
    if (ISR0 & BIT(1)) {
        // INT1: PWR_SW#
        power_handle_pwr_sw();
        ISR0 = BIT(1);
        WUESR2 = BIT(0);
    }
}
