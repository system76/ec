// SPDX-License-Identifier: GPL-3.0-only

#include <board/board.h>
#include <board/espi.h>
#include <board/gctrl.h>
#include <board/gpio.h>
#include <board/power.h>
#include <common/debug.h>
#include <ec/ec.h>

void board_init(void) {
    espi_init();

    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
}

void board_event(void) {
    power_set_limit();

    espi_event();

    ec_read_post_codes();
}
