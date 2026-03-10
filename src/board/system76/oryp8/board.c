// SPDX-License-Identifier: GPL-3.0-only

#include <app/board.h>
#include <board/gpio.h>
#include <ec/ec.h>

void board_init(void) {
    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
}

void board_event(void) {
    ec_read_post_codes();
}
