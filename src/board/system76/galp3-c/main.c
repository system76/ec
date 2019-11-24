#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <arch/delay.h>
#include <board/battery.h>
#include <board/gpio.h>
#include <board/gctrl.h>
#include <board/kbc.h>
#include <board/kbscan.h>
#include <board/peci.h>
#include <board/pmc.h>
#include <board/power.h>
#include <board/ps2.h>
#include <board/pwm.h>
#include <board/smbus.h>
#include <common/debug.h>
#include <common/macro.h>

uint8_t main_cycle = 0;

void external_0(void) __interrupt(0) {
    TRACE("external_0\n");
}

void timer_0(void) __interrupt(1) {
    TRACE("timer_0\n");
}

void external_1(void) __interrupt(2) {
    TRACE("external_1\n");
}

void timer_1(void) __interrupt(3) {
    TRACE("timer_1\n");
}

void serial(void) __interrupt(4) {
    TRACE("serial\n");
}

void timer_2(void) __interrupt(5) {
    TRACE("timer_2\n");
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

    // Allow PECI pin to be used
    GCR2 |= (1 << 4);
}

void ac_adapter() {
    static struct Gpio __code ACIN_N = GPIO(B, 6);
    static struct Gpio __code LED_ACIN = GPIO(C, 7);

    static bool last = true;

    // Check if the adapter line goes low
    bool new = gpio_get(&ACIN_N);
    // Set ACIN LED
    gpio_set(&LED_ACIN, !new);

    // If there has been a change, print
    if (new != last) {
        DEBUG("Power adapter ");
        if (new) {
            DEBUG("unplugged\n");
            battery_charger_disable();
        } else {
            DEBUG("plugged in\n");
            battery_charger_enable();
        }
        battery_debug();

        // Reset main loop cycle to force reading PECI and battery
        main_cycle = 0;
    }

    last = new;
}

void touchpad_event(struct Ps2 * ps2) {
    if (kbc_second) {
        *(ps2->control) = 0x07;
    } else {
        ps2_reset(ps2);
    }

    uint8_t status = *(ps2->status);
    *(ps2->status) = status;
    if (status & (1 << 3)) {
        uint8_t data = *(ps2->data);
        TRACE("touchpad: %02X\n", data);
        kbc_mouse(&KBC, data, 1000);
    }
}

struct Gpio __code LED_SSD_N = GPIO(G, 6);
struct Gpio __code LED_AIRPLANE_N = GPIO(G, 6);

void main(void) {
    init();

    INFO("\n");

    static struct Gpio __code LED_BAT_CHG =     GPIO(A, 5);
    static struct Gpio __code LED_BAT_FULL =    GPIO(A, 6);
    static struct Gpio __code SMI_N =           GPIO(D, 3);
    static struct Gpio __code SCI_N =           GPIO(D, 4);
    static struct Gpio __code SWI_N =           GPIO(E, 0);
    static struct Gpio __code SB_KBCRST_N =     GPIO(E, 6);
    static struct Gpio __code BT_EN =           GPIO(F, 3);
    static struct Gpio __code PM_CLKRUN_N =     GPIO(H, 0);
    static struct Gpio __code BKL_EN =          GPIO(H, 2);
    static struct Gpio __code WLAN_EN =         GPIO(H, 5);
    static struct Gpio __code WLAN_PWR_EN =     GPIO(J, 4);

    // Set the battery full LED (to know our firmware is loading)
    gpio_set(&LED_BAT_CHG, true);

#if GPIO_DEBUG
    gpio_debug();
#endif

    // Allow CPU to boot
    gpio_set(&SB_KBCRST_N, true);
    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable wireless
    gpio_set(&BT_EN, true);
    gpio_set(&WLAN_EN, true);
    gpio_set(&WLAN_PWR_EN, true);
    // Assert SMI#, SCI#, and SWI#
    gpio_set(&SCI_N, true);
    gpio_set(&SMI_N, true);
    gpio_set(&SWI_N, true);

    // Set the battery full LED (to know our firmware is loaded)
    gpio_set(&LED_BAT_FULL, true);
    INFO("Hello from System76 EC for %s!\n", xstr(__BOARD__));

    for(main_cycle = 0; ; main_cycle++) {
        // Enables or disables battery charging based on AC adapter
        ac_adapter();
        // Handle power states
        power_event();
        // Scans keyboard and sends keyboard packets
        kbscan_event();
        // Passes through touchpad packets
        touchpad_event(&PS2_3);
        // Checks for keyboard/mouse packets from host
        kbc_event(&KBC);
        // Only run the following once out of every 256 loops
        if (main_cycle == 0) {
            // Updates fan status and temps
            peci_event();
            // Updates battery status
            battery_event();
        }
        // Handles ACPI communication
        pmc_event(&PMC_1);
    }
}
