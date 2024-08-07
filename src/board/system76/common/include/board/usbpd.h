// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_USBPD_H
#define _BOARD_USBPD_H

#include <stdbool.h>
#include <stdint.h>

#if CONFIG_HAVE_USBPD

void usbpd_init(void);
void usbpd_event(void);
void usbpd_disable_charging(void);
void usbpd_enable_charging(void);
bool usbc_mux_info(uint8_t port, uint16_t *info);

#else

static inline void usbpd_init(void) {}
static inline void usbpd_event(void) {}
static inline void usbpd_disable_charging(void) {}
static inline void usbpd_enable_charging(void) {}

static inline bool usbc_mux_info(uint8_t port, uint16_t *info) {
    (void)port;
    (void)info;
    return false;
}

#endif

#endif // _BOARD_USBPD_H
