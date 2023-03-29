// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_USBPD_H
#define _BOARD_USBPD_H

void usbpd_init(void);
void usbpd_event(void);
void usbpd_disable_charging(void);
void usbpd_enable_charging(void);

#endif // _BOARD_USBPD_H
