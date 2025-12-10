// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2021 System76, Inc.

#include <app/board.h>
#include <app/espi.h>
#include <board/gpio.h>
#include <soc/soc.h>

void board_init(void) {
    espi_init();

    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
}

void board_event(void) {
    espi_event();

    soc_read_post_codes();
}
