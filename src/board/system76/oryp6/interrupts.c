// SPDX-License-Identifier: GPL-3.0-only

#include <board/interrupts.h>
#include <board/power.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/intc.h>
#include <ec/wuc.h>

static const struct {
    volatile uint8_t *wuesr;
    uint8_t wuc_bit;
    void (*isr)(void);
} irqs[] = {
    [17] = { &WUESR2, 4, power_handle_buf_plt_rst },
    [92] = { &WUESR8, 4, power_handle_lan_wakeup },
};

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
    uint8_t irq = interrupt_get_irq();

    if (irqs[irq].isr) {
        irqs[irq].isr();

        if (irqs[irq].wuesr) {
            // Acknowledge the WUC source
            *(irqs[irq].wuesr) = BIT(irqs[irq].wuc_bit);
        }
    } else {
        WARN("Unhandled interrupt: INT%d\n", irq);
    }

    // Must be acknowledged or cause an interrupt storm
    interrupt_clear(irq);
}
