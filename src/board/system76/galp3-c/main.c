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
#include <board/smbus.h>
#include <common/macro.h>

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
    smbus_init();

    //TODO: INTC, PECI

    // PECI information can be found here: https://www.intel.com/content/dam/www/public/us/en/documents/design-guides/core-i7-lga-2011-guide.pdf
}

void ac_adapter() {
    static struct Gpio __code ACIN_N = GPIO(B, 6);
    static struct Gpio __code LED_ACIN = GPIO(C, 7);

    static bool last = false;

    // Check if the adapter line goes low
    bool new = gpio_get(&ACIN_N);
    // Set ACIN LED
    gpio_set(&LED_ACIN, !new);

    // If there has been a change, print
    if (new != last) {
        printf("Power adapter ");
        if (new) {
            printf("unplugged\n");
        } else {
            printf("plugged in\n");
        }
    }

    last = new;
}

void power_button() {
    static struct Gpio __code PWR_SW_N = GPIO(D, 0);
    static struct Gpio __code PWR_BTN_N = GPIO(D, 5);
    //static struct Gpio __code DD_ON = GPIO(E, 4);

    static bool last = false;

    // Check if the power switch goes low
    bool new = gpio_get(&PWR_SW_N);
    if (!new && last) {
        printf("Power switch\n");

        battery_debug();
        //gpio_set(&DD_ON, true);
    }
    last = new;

    gpio_set(&PWR_BTN_N, new);
}

void touchpad_event(struct Ps2 * ps2) {
    //TODO
}

struct Gpio __code LED_PWR = GPIO(A, 7);
struct Gpio __code LED_SSD_N = GPIO(G, 6);
struct Gpio __code LED_AIRPLANE_N = GPIO(G, 6);

void main(void) {
    init();

    static struct Gpio __code LED_BAT_CHG = GPIO(A, 5);
    static struct Gpio __code LED_BAT_FULL = GPIO(A, 6);

    // Set the battery full LED (to know our firmware is loading)
    gpio_set(&LED_BAT_CHG, true);

    gpio_debug();

    battery_debug();

    // Set the battery full LED (to know our firmware is loaded)
    gpio_set(&LED_BAT_FULL, true);
    printf("Hello from System76 EC for %s!\n", xstr(__BOARD__));

    for(;;) {
        ac_adapter();
        power_button();
        touchpad_event(&PS2_3);
        kbc_event(&KBC);
        pmc_event(&PMC_1);
    }
}
