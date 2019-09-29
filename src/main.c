#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "include/delay.h"
#include "include/gpio.h"
#include "include/gctrl.h"
#include "include/kbc.h"
#include "include/pin.h"
#include "include/pmc.h"
#include "include/ps2.h"
#include "include/kbscan.h"

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

    //TODO: INTC, PECI, PWM, SMBUS

    // PECI information can be found here: https://www.intel.com/content/dam/www/public/us/en/documents/design-guides/core-i7-lga-2011-guide.pdf
}

struct Pin PWR_SW = PIN(D, 0);

void power_switch(void) {
    static bool last = false;

    // Check if the power switch goes low
    bool new = pin_get(&PWR_SW);
    if (!new && last) {
        printf("Power Switch\n");
    }
    last = new;
}

struct Pin LED_BAT_CHG = PIN(A, 5);
struct Pin LED_BAT_FULL = PIN(A, 6);
struct Pin LED_PWR = PIN(A, 7);
struct Pin LED_ACIN = PIN(C, 7);
struct Pin LED_AIRPLANE_N = PIN(G, 6);

__code const char * MODEL = "galp3-c";

void main(void) {
    init();

    // Set the battery full LED (to know our firmware is loaded)
    pin_set(&LED_BAT_FULL, true);
    printf("Hello from System76 EC for %s!\n", MODEL);

    gpio_debug();

    for(;;) {
        power_switch();
        kbc_event(&KBC);
        pmc_event(&PMC_1);
    }
}
