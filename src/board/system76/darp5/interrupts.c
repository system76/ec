// SPDX-License-Identifier: GPL-3.0-only

#include <board/interrupts.h>
#include <board/power.h>
#include <common/macro.h>
#include <ec/intc.h>
#include <ec/wuc.h>

void interrupts_init(void) {
    // INT17: BUF_PLT_RST#
    WUESR2 = BIT(4);
    interrupt_enable(17);

    // INT92: LAN_WAKEUP#
    WUEMR8 = BIT(4);
    WUESR8 = BIT(4);
    interrupt_enable(92);
}

void external_1(void) __interrupt(2) {
    if (interrupt_is_pending(17)) {
        // INT17: BUF_PLT_RST#
        power_handle_buf_plt_rst();
        interrupt_clear(17);
        WUESR2 = BIT(4);
    } else if (interrupt_is_pending(92)) {
        // INT92: LAN_WAKEUP#
        power_handle_lan_wakeup();
        interrupt_clear(92);
        WUESR8 = BIT(4);
    }
}
