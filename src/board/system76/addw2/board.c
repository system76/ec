// SPDX-License-Identifier: GPL-3.0-only

#include <board/board.h>
#include <board/gctrl.h>
#include <board/gpio.h>
#include <board/kbc.h>
#include <board/power.h>
#include <common/debug.h>
#include <ec/ec.h>

extern uint8_t main_cycle;

void board_init(void) {
    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
    // Assert SMI#, SCI#, and SWI#
    gpio_set(&SCI_N, true);
    gpio_set(&SMI_N, true);
    gpio_set(&SWI_N, true);
}

void board_event(void) {
    power_set_limit();

    ec_read_post_codes();

    if (main_cycle == 0) {
        // Set keyboard LEDs
        static uint8_t last_kbc_leds = 0;
        if (kbc_leds != last_kbc_leds) {
            gpio_set(&LED_SCROLL_N, (kbc_leds & 1) == 0);
            gpio_set(&LED_NUM_N, (kbc_leds & 2) == 0);
            gpio_set(&LED_CAP_N, (kbc_leds & 4) == 0);
            last_kbc_leds = kbc_leds;
        }
    }
}
