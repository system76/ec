// SPDX-License-Identifier: GPL-3.0-only

#include <board/interrupts.h>
#include <board/power.h>
#include <common/macro.h>
#include <ec/intc.h>
#include <ec/wuc.h>

// Board does not have:
// - LAN_WAKEUP#

void interrupts_init(void) {
    // INT17: BUF_PLT_RST#
    WUESR2 = BIT(4);
    ISR2 = BIT(1);
    IER2 = BIT(1);
}

void external_1(void) __interrupt(2) {
    if (ISR2 & BIT(1)) {
        // INT17: BUF_PLT_RST#
        power_handle_buf_plt_rst();
        ISR2 = BIT(1);
        WUESR2 = BIT(4);
    } else if (ISR19 & BIT(7)) {
        // INT159: PLL Frequency Change Event
        // TODO: What needs to be done?
        ISR19 = BIT(7);
    }
}
