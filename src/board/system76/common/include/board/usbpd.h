// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_USBPD_H
#define _BOARD_USBPD_H

void usbpd_init(void);
void usbpd_event(void);
void usbpd_disable_charging(void);
void usbpd_enable_charging(void);
bool usbc_mux_info(uint8_t port, uint16_t *info);

#endif // _BOARD_USBPD_H
