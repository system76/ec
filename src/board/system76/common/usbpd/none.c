// SPDX-License-Identifier: GPL-3.0-only

#include <board/usbpd.h>
#include <common/macro.h>

void usbpd_init(void) {}

void usbpd_event(void) {}

void usbpd_disable_charging(void) {}

void usbpd_enable_charging(void) {}

bool usbc_mux_info(uint8_t port, uint16_t *info) {
    MAYBE_UNUSED(port);
    MAYBE_UNUSED(info);

    return false;
}
