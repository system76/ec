#include <8051.h>
#include <stdbool.h>
#include <stdio.h>

#include "include/delay.h"
#include "include/gpio.h"
#include "include/gctrl.h"
#include "include/kbc.h"
#include "include/pin.h"
#include "include/pmc.h"
#include "include/ps2.h"
#include "include/kbscan.h"

struct Pin LED_BAT_CHG = PIN(A, 5);
struct Pin LED_BAT_FULL = PIN(A, 6);
struct Pin LED_PWR = PIN(A, 7);
struct Pin LED_ACIN = PIN(C, 7);
struct Pin LED_AIRPLANE_N = PIN(G, 6);

struct Pin PWR_SW = PIN(D, 0);

void main() {
    gpio_init();
    gctrl_init();
    kbc_init();
    pmc_init();
    kbscan_init();

    //TODO: INTC, PECI, PWM, SMBUS

    // Set the battery full LED (to know our firmware is loaded)
    pin_set(&LED_BAT_CHG, true);
    delay_ms(1000);
    pin_set(&LED_BAT_FULL, true);
    printf("Hello from System76 EC!\n");

    bool last = false;
    for(;;) {
        // Check if the power switch goes low
        bool new = pin_get(&PWR_SW);
        if (!new && last) {
            printf("Power Switch\n");
        }
        last = new;
    }
}
