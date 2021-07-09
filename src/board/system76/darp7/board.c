// SPDX-License-Identifier: GPL-3.0-only

#include <board/board.h>
#include <board/espi.h>
#include <board/gctrl.h>
#include <board/gpio.h>
#include <common/debug.h>
#include <ec/ec.h>

void board_init(void) {
    espi_init();

    // Allow CPU to boot
    gpio_set(&SB_KBCRST_N, true);
    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
    // Enable wireless
    gpio_set(&WLAN_EN, true);
    gpio_set(&WLAN_PWR_EN, true);
    // Enable right USB port
    gpio_set(&USB_PWR_EN_N, false);
    // Assert SMI# and SWI#
    gpio_set(&SMI_N, true);
    gpio_set(&SWI_N, true);
}

void board_event(void) {
    espi_event();

    ec_read_post_codes();
}
