#include <board/battery.h>
#include <board/board.h>
#include <board/gpio.h>

extern uint8_t main_cycle;

void board_init(void) {
    // Allow CPU to boot
    gpio_set(&SB_KBCRST_N, true);
    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
    // Enable wireless
    gpio_set(&BT_EN, true);
    gpio_set(&WLAN_EN, true);
    gpio_set(&WLAN_PWR_EN, true);
    // Enable right USB port
    gpio_set(&USB_PWR_EN_N, false);
    // Assert SMI#, SCI#, and SWI#
    gpio_set(&SCI_N, true);
    gpio_set(&SMI_N, true);
    gpio_set(&SWI_N, true);
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
    }
}
