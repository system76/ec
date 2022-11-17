// SPDX-License-Identifier: GPL-3.0-only

#include <board/board.h>
#include <board/gctrl.h>
#include <board/gpio.h>
#include <board/power.h>
#include <board/wireless.h>
#include <common/debug.h>
#include <ec/ec.h>

void board_init(void) {
    // Allow CPU to boot
    gpio_set(&SB_KBCRST_N, true);
    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
    // Enable USB port power?
    gpio_set(&USB_PWR_EN_N, false);
    // Assert SMI#, SCI#, and SWI#
    gpio_set(&SCI_N, true);
    gpio_set(&SMI_N, true);
    gpio_set(&SWI_N, true);

    wireless_power(true);
}

void board_event(void) {
    power_set_limit();

    ec_read_post_codes();
}
