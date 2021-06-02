// SPDX-License-Identifier: GPL-3.0-only

#include <board/interrupts.h>
#include <board/power.h>
#include <common/debug.h>
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
    uint8_t irq = interrupt_get_irq();

    switch (irq) {
    case 17:
        // INT17: BUF_PLT_RST#
        power_handle_buf_plt_rst();
        WUESR2 = BIT(4);
        break;

    default:
        WARN("Unhandled interrupt: INT%d\n", irq);
        break;
    }

    // XXX: Must be acknowledged or cause an interrupt storm
    interrupt_clear(irq);
}
