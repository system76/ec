#include <arch/delay.h>
#include <board/espi.h>
#include <board/gpio.h>
#include <board/power.h>
#include <common/debug.h>

void power_on_ds5(void) {
    DEBUG("custom power_on_ds5\n");
    delay_ms(84);
}

void power_on_s5(void) {
    DEBUG("custom power_on_s5\n");
    gpio_set(&PCH_DPWROK_EC, true);
    delay_ms(95);
    gpio_set(&VA_EC_EN, true);
    delay_us(200);
    //TODO: where should this be? It cannot allow SLP_S* to glitch
    gpio_set(&EC_EN, true);
    gpio_set(&DD_ON, true);
    delay_ms(143);
    gpio_set(&EC_RSMRST_N, true);
    //TODO: find out exact requirements
    for (int i = 0; i < 100; i++) {
        espi_event();
        delay_ms(1);
    }
    gpio_set(&PWR_BTN_N, false);
    //TODO: verify timing
    delay_ms(100);
    gpio_set(&PWR_BTN_N, true);
    // System should continue powering on S5, S4, and S3 planes
}

void power_off_s5(void) {
    DEBUG("custom power_off_s5\n");
    //TODO: verify timing
    gpio_set(&PCH_PWROK_EC, false);
    gpio_set(&PM_PWROK, false);
    gpio_set(&EC_RSMRST_N, false);
    gpio_set(&DD_ON, false);
    gpio_set(&EC_EN, false);
    delay_us(1);
    gpio_set(&VA_EC_EN, false);
    gpio_set(&PCH_DPWROK_EC, false);
}
