#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <arch/delay.h>
#include <board/battery.h>
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
    static struct Gpio __code PCH_DPWROK_EC =   GPIO(A, 3);
    static struct Gpio __code PCH_PWROK_EC =    GPIO(A, 4);
    static struct Gpio __code LED_PWR =         GPIO(A, 7);
    static struct Gpio __code ALL_SYS_PWRGD =   GPIO(C, 0);
    static struct Gpio __code PWR_SW_N =        GPIO(D, 0);
    static struct Gpio __code PWR_BTN_N =       GPIO(D, 5);
    static struct Gpio __code EC_EN =           GPIO(E, 1);
    static struct Gpio __code DD_ON =           GPIO(E, 4);
    static struct Gpio __code EC_RSMRST_N =     GPIO(E, 5);
    static struct Gpio __code VA_EC_EN =        GPIO(E, 7);
    static struct Gpio __code SUSC_N_PCH =      GPIO(H, 1);
    static struct Gpio __code VR_ON =           GPIO(H, 4);
    static struct Gpio __code SUSB_N_PCH =      GPIO(H, 6);
    static struct Gpio __code SLP_SUS_N =       GPIO(I, 2);
    static struct Gpio __code SUS_PWR_ACK =     GPIO(J, 0);

    // Check if the power switch goes low
    static bool last = false;
    bool new = gpio_get(&PWR_SW_N);
    if (!new && last) {
        printf("Power switch press\n");

        static bool power = false;
        power = !power;

        if (power) {
            printf("Enabling power\n");

            // We assume that VCCRTC has already been stable, RTCRST# is
            // already set, and VCCDSW_3P3 is stable

            // Enable battery charger - also provides correct power levels for
            // system boot sourced from the AC adapter
            battery_charger_enable();

            // Make sure VCCDSW is stable for at least 10 ms (tPCH02)
            delay_ms(10 + 5);

            // Assert DSW_PWROK
            printf("PCH_DPWROK_EC: %d\n", power);
            gpio_set(&PCH_DPWROK_EC, power);

            // Wait for SLP_SUS# (tPCH32)
            delay_ms(95);
            for (;;) {
                bool slp_sus = gpio_get(&SLP_SUS_N);
                printf("SLP_SUS_N: %d\n", slp_sus);
                if (slp_sus) break;
                delay_ms(1);
            }

            // Enable VCCPRIM_* planes - must be enabled prior to USB power
            // in order to avoid leakage
            printf("VA_EC_EN: %d\n", power);
            gpio_set(&VA_EC_EN, power);

            // Make sure VCCPRIM_* is stable for at least 10 ms (tPCH03)
            delay_ms(10 + 5);

            // Enable VDD5
            printf("DD_ON: %d\n", power);
            gpio_set(&DD_ON, power);

            // Assert RSMRST#
            printf("EC_RSMRST_N: %d\n", power);
            gpio_set(&EC_RSMRST_N, power);

            // printf("SUS_PWR_ACK: %d\n", power);
            // gpio_set(&SUS_PWR_ACK, power);
            //
            // // Wait for tPCH03
            // delay_ms(10);
            //
            // printf("

            // printf("EC_EN: %d\n", power);
            // gpio_set(&EC_EN, power);
            //
            // printf("VR_ON: %d\n", power);
            // gpio_set(&VR_ON, power);

            printf("LED_PWR: %d\n", power);
            gpio_set(&LED_PWR, power);
        } else {
            //TODO
            printf("Disabling power\n");

            printf("LED_PWR: %d\n", power);
            gpio_set(&LED_PWR, power);

            // De-assert RSMRST#
            printf("EC_RSMRST_N: %d\n", power);
            gpio_set(&EC_RSMRST_N, power);

            // Disable VDD5
            printf("DD_ON: %d\n", power);
            gpio_set(&DD_ON, power);

            // Wait a minimum of 400 ns (tPCH12)
            delay_ms(1);

            // Disable VCCPRIM_* planes
            printf("VA_EC_EN: %d\n", power);
            gpio_set(&VA_EC_EN, power);

            // De-assert DSW_PWROK
            printf("PCH_DPWROK_EC: %d\n", power);
            gpio_set(&PCH_DPWROK_EC, power);

            // Wait a minimum of 400 ns (tPCH14)
            delay_ms(1);

            // Disable battery charger
            battery_charger_disable();
        }
    } else if (new && !last) {
        printf("Power switch release\n");

        printf("ALL_SYS_PWRGD: %d\n", gpio_get(&ALL_SYS_PWRGD));

        battery_debug();
    }
    last = new;

    gpio_set(&PWR_BTN_N, new);
}

void touchpad_event(struct Ps2 * ps2) {
    //TODO
}

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
