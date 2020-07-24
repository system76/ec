#include <board/board.h>
#include <board/gctrl.h>
#include <board/gpio.h>
#include <common/debug.h>

extern uint8_t main_cycle;

void board_init(void) {
    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
    // Enable wireless
    gpio_set(&BT_EN, true);
    gpio_set(&WLAN_EN, true);
    gpio_set(&WLAN_PWR_EN, true);
    // Assert SMI#, SCI#, and SWI#
    gpio_set(&SCI_N, true);
    gpio_set(&SMI_N, true);
    gpio_set(&SWI_N, true);

    // Enable POST codes
    SPCTRL1 |= 0xC8;
}

void board_on_ac(bool ac) {
    // Use parameter
    ac = ac;
}

void board_event(void) {
    // Read POST codes
    while (P80H81HS & 1) {
        uint8_t p80h = P80HD;
        uint8_t p81h = P81HD;
        P80H81HS |= 1;

        DEBUG("POST %02X%02X\n", p81h, p80h);
    }
}
