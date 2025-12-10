// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2024 System76, Inc.

#include <app/board.h>
#include <app/battery.h>
#include <app/espi.h>
#include <board/gpio.h>
#include <ec/ec.h>

void board_init(void) {
    espi_init();

    // Make sure charger is in off state, also enables PSYS
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
