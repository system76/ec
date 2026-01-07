// SPDX-License-Identifier: GPL-3.0-only

#include <app/wireless.h>
#include <board/gpio.h>

#ifndef HAVE_BT_EN
#define HAVE_BT_EN 1
#endif
#ifndef HAVE_WLAN_EN
#define HAVE_WLAN_EN 1
#endif
#ifndef HAVE_WLAN_PWR_EN
#define HAVE_WLAN_PWR_EN 1
#endif

/**
 * Set the WLAN card power state.
 */
void wireless_power(bool enable) {
    MAYBE_UNUSED(enable);

#if HAVE_BT_EN
    gpio_set(&BT_EN, enable);
#endif
#if HAVE_WLAN_EN
    gpio_set(&WLAN_EN, enable);
#endif
#if HAVE_WLAN_PWR_EN
    gpio_set(&WLAN_PWR_EN, enable);
#endif
}
