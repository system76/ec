#include <board/board.h>
#include <board/gctrl.h>
#include <board/gpio.h>
#include <board/power.h>

extern uint8_t main_cycle;

void board_init(void) {}

void board_event(void) {
    if (main_cycle == 0) {
        if (power_state == POWER_STATE_S0 || power_state == POWER_STATE_S3 || power_state == POWER_STATE_DS3) {
            // System is on
        } else if (gpio_get(&ACIN_N)) {
            // Power off VDD3 if system should be off
            gpio_set(&XLP_OUT, 0);
        }
    }
}
