// SPDX-License-Identifier: GPL-3.0-only

#include <board/interrupts.h>
#include <board/power.h>
#include <common/macro.h>
#include <ec/intc.h>
#include <ec/wuc.h>

void interrupts_init(void) {
    // XXX: Can't set debounce for PWR_SW# on IT5570E?

    // INT108: PWR_SW#
    WUEMR10 = BIT(7); // Either-edge
    WUESR10 = BIT(7);
    ISR13 = BIT(4);
    IER13 = BIT(4);
}

void external_1(void) __interrupt(2) {
    if (ISR13 & BIT(4)) {
        // INT108: PWR_SW#
        power_handle_pwr_sw();
        ISR13 = BIT(4);
        WUESR10 = BIT(7);
    }
}
