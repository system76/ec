// SPDX-License-Identifier: GPL-3.0-only

#include <board/board.h>
#include <board/battery.h>
#include <board/gpio.h>
#include <board/espi.h>
#include <common/debug.h>
#include <ec/adc.h>
#include <ec/ec.h>

void board_init(void) {
    espi_init();
    battery_charger_disable();

    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);

    adc_init();
}

uint8_t board_id(void) {
    static bool cached = false;
    static uint8_t board_id = 0;

    if (cached)
        return board_id;

    int16_t id = adc_read_channel(7);
    if (id < 0) {
        ERROR("Board ID error %ld, defaulting to V54 GN20P\n", id);
        board_id = 0;
        goto exit;
    }

    if (id < 250) {
        INFO("Board ID: V54 GN20P\n");
        board_id = 0;
        goto exit;
    }

    if (id < 800) {
        INFO("Board ID: V56 GN20P\n");
        board_id = 1;
        goto exit;
    }

    if (id < 1800) {
        INFO("Board ID: V54 GN21-X4/X2\n");
        board_id = 2;
        goto exit;
    }

    if (id < 2400) {
        INFO("Board ID: V54 GN21-X6\n");
        board_id = 3;
        goto exit;
    }

    if (id < 2800) {
        INFO("Board ID: V56 GN21-X4/X2\n");
        board_id = 4;
        goto exit;
    }

    INFO("Board ID: V56 GN21-X6\n");
    board_id = 5;

exit:
    cached = true;
    return board_id;
}

void board_event(void) {
    espi_event();

    board_id();

    ec_read_post_codes();
}
