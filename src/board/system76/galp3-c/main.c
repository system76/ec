#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <arch/delay.h>
#include <board/gpio.h>
#include <board/gctrl.h>
#include <board/kbc.h>
#include <board/kbscan.h>
#include <board/pmc.h>
#include <board/ps2.h>
#include <board/pwm.h>

void external_0(void) __interrupt(0) {
    printf("external_0\n");
}

void timer_0(void) __interrupt(1) {
    printf("timer_0\n");
}

void external_1(void) __interrupt(2) {
    printf("external_1\n");
}

void timer_1(void) __interrupt(3) {
    printf("timer_1\n");
}

void serial(void) __interrupt(4) {
    printf("serial\n");
}

void timer_2(void) __interrupt(5) {
    printf("timer_2\n");
}

void init(void) {
    gpio_init();
    gctrl_init();
    kbc_init();
    pmc_init();
    kbscan_init();
    pwm_init();

    //TODO: INTC, PECI, SMBUS

    // PECI information can be found here: https://www.intel.com/content/dam/www/public/us/en/documents/design-guides/core-i7-lga-2011-guide.pdf
}

void power_button(struct Gpio * button) {
    static bool last = false;

    // Check if the power switch goes low
    bool new = gpio_get(button);
    if (!new && last) {
        printf("Power Switch\n");
    }
    last = new;
}

struct Gpio PWR_SW = GPIO(D, 0);

struct Gpio LED_BAT_CHG = GPIO(A, 5);
struct Gpio LED_BAT_FULL = GPIO(A, 6);
struct Gpio LED_PWR = GPIO(A, 7);
struct Gpio LED_ACIN = GPIO(C, 7);
struct Gpio LED_AIRPLANE_N = GPIO(G, 6);

__code const char * MODEL = "galp3-c";

void main(void) {
    init();

    gpio_debug();

    // Set the battery full LED (to know our firmware is loaded)
    gpio_set(&LED_BAT_FULL, true);
    printf("Hello from System76 EC for %s!\n", MODEL);

    for(;;) {
        power_button(&PWR_SW);
        kbc_event(&KBC);
        pmc_event(&PMC_1);
    }
}
