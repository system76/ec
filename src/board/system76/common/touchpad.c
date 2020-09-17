// SPDX-License-Identifier: GPL-3.0-only

#include <board/kbc.h>
#include <board/touchpad.h>
#include <common/debug.h>
#include <ec/ps2.h>

void touchpad_event(void) {
    if (kbc_second) {
        *(PS2_TOUCHPAD.control) = 0x07;
    } else {
        ps2_reset(&PS2_TOUCHPAD);
    }

    uint8_t status = *(PS2_TOUCHPAD.status);
    *(PS2_TOUCHPAD.status) = status;
    if (status & (1 << 3)) {
        uint8_t data = *(PS2_TOUCHPAD.data);
        TRACE("touchpad: %02X\n", data);
        kbc_mouse(&KBC, data, 1000);
    }
}
