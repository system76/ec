// SPDX-License-Identifier: GPL-3.0-only

#include <board/interrupts.h>
#include <board/power.h>
#include <common/macro.h>
#include <ec/intc.h>
#include <ec/wuc.h>

// Board does not have:
// - LAN_WAKEUP#

void interrupts_init(void) {
}

void external_1(void) __interrupt(2) {
}
