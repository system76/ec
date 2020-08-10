#include <board/board.h>
#include <board/gctrl.h>
#include <board/gpio.h>
#include <board/pnp.h>
#include <common/debug.h>
#include <ec/bram.h>

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

void board_on_cpu_reset(void) {
    // Set up ring-buffer for POST codes
    for (uint8_t i = 0x80; i <= 0xBF; i++) {
        BRAM[i] = 0;
    }
    pnp_write(0x07, 0x10);
    pnp_write(0xF3, 0x00);
    pnp_write(0xF4, 0x3F);
}

void board_event(void) {
    // Read POST codes
    while (P80H81HS & 1) {
        uint8_t p80h = P80HD;
        uint8_t p81h = P81HD;
        P80H81HS |= 1;

        DEBUG("POST %02X%02X:", p81h, p80h);

        // Also dump ring-buffer
        for (uint8_t i = 0x80; i <= 0xBF; i+=2) {
            DEBUG(" %02X%02X", BRAM[i + 1], BRAM[i]);
        }
        DEBUG("\n");
    }
}
