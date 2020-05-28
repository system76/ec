#include <board/battery.h>
#include <board/board.h>
#include <board/gctrl.h>
#include <board/gpio.h>
#include <board/kbc.h>
#include <board/power.h>

extern uint8_t main_cycle;

void board_init(void) {
    RSTS = 0x44;
}

void board_event(void) {
    static uint8_t last_kbc_leds = 0;

    if (main_cycle == 0) {
        if (gpio_get(&ACIN_N)) {
            // Discharging (no AC adapter)
            gpio_set(&LED_BAT_CHG, false);
            gpio_set(&LED_BAT_FULL, false);
        } else if (battery_status & 0x0020) {
            // Fully charged
            // TODO: turn off charger
            gpio_set(&LED_BAT_CHG, false);
            gpio_set(&LED_BAT_FULL, true);
        } else {
            // Charging
            // TODO: detect no battery connected
            gpio_set(&LED_BAT_CHG, true);
            gpio_set(&LED_BAT_FULL, false);
        }

        if (power_state == POWER_STATE_S0 || power_state == POWER_STATE_S3 || power_state == POWER_STATE_DS3) {
            // System is on
        } else if (gpio_get(&ACIN_N)) {
            // Power off VDD3 if system should be off
            gpio_set(&XLP_OUT, 0);
        }
    }

    // Set keyboard LEDs
    if (kbc_leds != last_kbc_leds) {
        gpio_set(&LED_SCROLL_N, (kbc_leds & 1) == 0);
        gpio_set(&LED_NUM_N, (kbc_leds & 2) == 0);
        gpio_set(&LED_CAP_N, (kbc_leds & 4) == 0);
        last_kbc_leds = kbc_leds;
    }
}
