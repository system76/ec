// SPDX-License-Identifier: GPL-3.0-only

#include <board/battery.h>
#include <board/board.h>
#include <board/espi.h>
#include <board/gpio.h>
#include <board/wireless.h>
#include <ec/ec.h>

void board_init(void) {
    espi_init();

    // Make sure charger is in off state, also enables PSYS
    battery_charger_disable();

    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
    // Enable USB port power
    gpio_set(&USB_PWR_EN_N, false);

    wireless_power(true);
}

void board_event(void) {
    espi_event();

    ec_read_post_codes();
}
