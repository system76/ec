// SPDX-License-Identifier: GPL-3.0-only

#include <board/battery.h>
#include <board/board.h>
#include <board/espi.h>
#include <board/gpio.h>
#include <ec/ec.h>

void board_init(void) {
    espi_init();

    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
    // Enable wireless
    gpio_set(&WLAN_EN, true);
    gpio_set(&WLAN_PWR_EN, true);
    // Assert SMI# and SWI#
    gpio_set(&SMI_N, true);
    gpio_set(&SWI_N, true);

    // Make sure charger is in off state, also enables PSYS
    battery_charger_disable();
}

void board_event(void) {
    espi_event();

    ec_read_post_codes();
}
