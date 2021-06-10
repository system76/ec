// SPDX-License-Identifier: GPL-3.0-only

#include <arch/time.h>
#include <board/battery.h>
#include <board/board.h>
#include <board/dgpu.h>
#include <board/gctrl.h>
#include <board/gpio.h>
#include <board/kbc.h>
#include <board/power.h>
#include <common/debug.h>

extern uint8_t main_cycle;

void board_init(void) {
    // Allow CPU to boot
    gpio_set(&SB_KBCRST_N, true);
    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
    // Enable wireless
    gpio_set(&BT_EN, true);
    gpio_set(&WLAN_EN, true);
    gpio_set(&WLAN_PWR_EN, true);
    // Enable USB power
    gpio_set(&USB_PWR_EN_N, false);
    // Assert SMI#, SCI#, and SWI#
    gpio_set(&SCI_N, true);
    gpio_set(&SMI_N, true);
    gpio_set(&SWI_N, true);
}

void board_event(void) {
    power_set_limit();
}
