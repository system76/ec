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
        ERROR("Board ID error %ld, defaulting to V560TU\n", id);
        board_id = 0;
        goto exit;
    }

    if (id < 200) {
        INFO("Board ID: V560TU\n");
        board_id = 0;
        goto exit;
    }

    if (id < 2800) {
        WARN("Board ID: Unrecognized %ld, defaulting to V560TU\n");
        board_id = 0;
        goto exit;
    }

    INFO("Board ID: V540TU\n");
    board_id = 1;

exit:
    cached = true;
    return 1;

}

void board_event(void) {
    espi_event();

    board_id();

    ec_read_post_codes();
}
