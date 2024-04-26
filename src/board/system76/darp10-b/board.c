// SPDX-License-Identifier: GPL-3.0-only

#include <board/board.h>
#include <board/battery.h>
#include <board/gpio.h>
#include <board/espi.h>
#include <ec/ec.h>

void board_init(void) {
    espi_init();
    battery_charger_disable();

    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
}

void board_event(void) {
    espi_event();

    ec_read_post_codes();
}
