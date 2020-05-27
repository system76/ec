#include <board/battery.h>
#include <board/board.h>
#include <board/gctrl.h>
#include <board/gpio.h>
#include <board/power.h>

extern uint8_t main_cycle;

void board_init(void) {
    RSTS = 0x44;
}

void board_event(void) {
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
}
