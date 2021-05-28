// SPDX-License-Identifier: GPL-3.0-only

#include <board/interrupts.h>
#include <board/power.h>
#include <common/macro.h>
#include <ec/intc.h>
#include <ec/wuc.h>

void interrupts_init(void) {
    // INT85: LAN_WAKEUP#
    WUEMR9 = BIT(0);
    WUESR9 = BIT(0);
    ISR10 = BIT(5);
    IER10 = BIT(5);
}

void external_1(void) __interrupt(2) {
    if (ISR10 & BIT(5)) {
        // INT85: LAN_WAKEUP#
        power_handle_lan_wakeup();
        ISR10 = BIT(5);
        WUESR9 = BIT(0);
    } else if (ISR19 & BIT(7)) {
        // INT159: PLL Frequency Change Event
        // TODO: What needs to be done?
        ISR19 = BIT(7);
    }
}
