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
    interrupt_enable(17);
}

void external_1(void) __interrupt(2) {
    if (interrupt_is_pending(17)) {
        // INT17: BUF_PLT_RST#
        power_handle_buf_plt_rst();
        interrupt_clear(17);
        WUESR2 = BIT(4);
    } else if (interrupt_is_pending(159)) {
        // INT159: PLL Frequency Change Event
        // TODO: What needs to be done?
        interrupt_clear(159);
    }
}
