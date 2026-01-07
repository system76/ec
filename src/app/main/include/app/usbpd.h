// SPDX-License-Identifier: GPL-3.0-only

#ifndef _APP_USBPD_H
#define _APP_USBPD_H

#if CONFIG_HAVE_USBPD

void usbpd_init(void);
void usbpd_event(void);
void usbpd_disable_charging(void);
void usbpd_enable_charging(void);

#else

static inline void usbpd_init(void) {}
static inline void usbpd_event(void) {}
static inline void usbpd_disable_charging(void) {}
static inline void usbpd_enable_charging(void) {}

#endif

#endif // _APP_USBPD_H
