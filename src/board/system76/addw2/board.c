#include <arch/time.h>
#include <board/battery.h>
#include <board/board.h>
#include <board/dgpu.h>
#include <board/gctrl.h>
#include <board/gpio.h>
#include <board/kbc.h>
#include <board/power.h>
#include <common/debug.h>

extern uint8_t main_cycle;

void board_init(void) {
    dgpu_init();
}

void board_event(void) {
    // Read POST codes
    if (P80H81HS & 1) {
        uint8_t p80h = P80HD;
        uint8_t p81h = P81HD;
        P80H81HS |= 1;

        DEBUG("POST %02X%02X\n", p81h, p80h);
    }

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

        static uint32_t last_time = 0;
        uint32_t time = time_get();
        // Only run the following once a second
        if (last_time > time || (time - last_time) >= 1000) {
            last_time = time;

            // Updates discrete GPU fan status and temps
            dgpu_event();
        }
    }

    // Set keyboard LEDs
    static uint8_t last_kbc_leds = 0;
    if (kbc_leds != last_kbc_leds) {
        gpio_set(&LED_SCROLL_N, (kbc_leds & 1) == 0);
        gpio_set(&LED_NUM_N, (kbc_leds & 2) == 0);
        gpio_set(&LED_CAP_N, (kbc_leds & 4) == 0);
        last_kbc_leds = kbc_leds;
    }
}
