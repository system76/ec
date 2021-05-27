// SPDX-License-Identifier: GPL-3.0-only

#include <board/interrupts.h>
#include <board/power.h>
#include <common/macro.h>
#include <ec/intc.h>
#include <ec/wuc.h>

void interrupts_init(void) {
    // INT17: BUF_PLT_RST#
    WUESR2 = BIT(4);
    ISR2 = BIT(1);
    IER2 = BIT(1);

    // INT92: LAN_WAKEUP#
    WUEMR8 = BIT(4);
    WUESR8 = BIT(4);
    ISR11 = BIT(4);
    IER11 = BIT(4);
}

void external_1(void) __interrupt(2) {
    if (ISR2 & BIT(1)) {
        // INT17: BUF_PLT_RST#
        power_handle_buf_plt_rst();
        ISR2 = BIT(1);
        WUESR2 = BIT(4);
    } else if (ISR11 & BIT(4)) {
        // INT92: LAN_WAKEUP#
        power_handle_lan_wakeup();
        ISR11 = BIT(4);
        WUESR8 = BIT(4);
    }
}
