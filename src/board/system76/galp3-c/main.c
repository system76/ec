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
#include <board/ps2.h>
#include <board/pwm.h>
#include <board/smbus.h>
#include <common/debug.h>
#include <common/macro.h>

static uint8_t main_cycle = 0;

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

volatile uint8_t __xdata __at(0x1200) IHIOA;
volatile uint8_t __xdata __at(0x1201) IHD;
volatile uint8_t __xdata __at(0x1204) IBMAE;
volatile uint8_t __xdata __at(0x1205) IBCTL;
void e2ci_write(uint8_t port, uint8_t data) {
    while (IBCTL & ((1 << 2) | (1 << 1))) {}
    IHIOA = port;
    IHD = data;
    IBMAE = 1;
    IBCTL = 1;
    while (IBCTL & (1 << 2)) {}
    IBMAE = 0;
    IBCTL = 0;
}

void pnp_write(uint8_t reg, uint8_t data) {
    e2ci_write(0x2E, reg);
    e2ci_write(0x2F, data);
}

void pnp_enable() {
    DEBUG("Enable PNP devices\n");

    // Enable PMC
    pnp_write(0x07, 0x11);
    pnp_write(0x30, 0x01);

    // Enable KBC keyboard
    pnp_write(0x07, 0x06);
    pnp_write(0x30, 0x01);

    // Enable KBC mouse
    pnp_write(0x07, 0x05);
    pnp_write(0x30, 0x01);

    // Enable SWUC
    pnp_write(0x07, 0x04);
    pnp_write(0x30, 0x01);
}

void power_button() {
    static struct Gpio __code PCH_DPWROK_EC =   GPIO(A, 3);
    static struct Gpio __code PCH_PWROK_EC =    GPIO(A, 4);
    static struct Gpio __code LED_PWR =         GPIO(A, 7);
    static struct Gpio __code ALL_SYS_PWRGD =   GPIO(C, 0);
    static struct Gpio __code PM_PWROK =        GPIO(C, 6);
    static struct Gpio __code PWR_SW_N =        GPIO(D, 0);
    static struct Gpio __code BUF_PLT_RST_N =   GPIO(D, 2);
    static struct Gpio __code PWR_BTN_N =       GPIO(D, 5);
    static struct Gpio __code SUSWARN_N =       GPIO(D, 7);
    static struct Gpio __code EC_EN =           GPIO(E, 1);
    static struct Gpio __code VA_EC_EN =        GPIO(E, 3);
    static struct Gpio __code DD_ON =           GPIO(E, 4);
    static struct Gpio __code EC_RSMRST_N =     GPIO(E, 5);
    static struct Gpio __code AC_PRESENT =      GPIO(E, 7);
    static struct Gpio __code SUSC_N_PCH =      GPIO(H, 1);
    static struct Gpio __code VR_ON =           GPIO(H, 4);
    static struct Gpio __code SUSB_N_PCH =      GPIO(H, 6);
    static struct Gpio __code SLP_SUS_N =       GPIO(I, 2);
    static struct Gpio __code SUS_PWR_ACK =     GPIO(J, 0);

    static bool power = false;

    // Check if the power switch goes low
    static bool last = true;
    bool new = gpio_get(&PWR_SW_N);
    if (!new && last) {
        // Ensure press is not spurious
        delay_ms(10);
        if (gpio_get(&PWR_SW_N) != new) {
            DEBUG("Spurious press\n");
            return;
        }

        DEBUG("Power switch press\n");

        power = !power;

        if (power) {
            DEBUG("Enabling S5 power\n");

            // We assume that VCCRTC has already been stable, RTCRST# is
            // already set, and VCCDSW_3P3 is stable

            // Make sure VCCDSW is stable for at least 10 ms (tPCH02)
            delay_ms(10 + 5);

            // Assert DSW_PWROK
            TRACE("PCH_DPWROK_EC: %d\n", power);
            gpio_set(&PCH_DPWROK_EC, power);

            // Wait for SLP_SUS# (tPCH32)
            delay_ms(95);
            for (;;) {
                bool value = gpio_get(&SLP_SUS_N);
                TRACE("SLP_SUS_N: %d\n", value);
                if (value) break;
                delay_ms(1);
            }

            // Enable VCCPRIM_* planes - must be enabled prior to USB power
            // in order to avoid leakage
            TRACE("VA_EC_EN: %d\n", power);
            gpio_set(&VA_EC_EN, power);

            // Make sure VCCPRIM_* is stable for at least 10 ms (tPCH03)
            delay_ms(10 + 5);

            // Enable VDD5
            TRACE("DD_ON: %d\n", power);
            gpio_set(&DD_ON, power);

            // Assert RSMRST#
            TRACE("EC_RSMRST_N: %d\n", power);
            gpio_set(&EC_RSMRST_N, power);

            // Allow processor to control SUSB# and SUSC#
            TRACE("EC_EN: %d\n", power);
            gpio_set(&EC_EN, power);

            // Assert SUS_ACK#
            TRACE("SUS_PWR_ACK: %d\n", power);
            gpio_set(&SUS_PWR_ACK, power);

            // DEBUG("VR_ON: %d\n", power);
            // gpio_set(&VR_ON, power);
        }
    }

    // Set PWR_BTN line!
    gpio_set(&PWR_BTN_N, new);

    if (!new && last) {
        if (power) {
            DEBUG("Enabling S0 power\n");

            // Wait for ALL_SYS_PWRGD
            for (;;) {
                bool value = gpio_get(&ALL_SYS_PWRGD);
                TRACE("ALL_SYS_PWRGD: %d\n", value);
                if (value) break;
                delay_ms(1);
            }

            // Assert VR_ON
            TRACE("VR_ON: %d\n", power);
            gpio_set(&VR_ON, power);

            // Assert PM_PWEROK, PCH_PWROK will be asserted when H_VR_READY is
            TRACE("PM_PWROK: %d\n", power);
            gpio_set(&PM_PWROK, power);

            // OEM defined delay from ALL_SYS_PWRGD to SYS_PWROK - TODO
            delay_ms(10);

            // Assert PCH_PWEROK_EC, SYS_PWEROK will be asserted
            TRACE("PCH_PWROK_EC: %d\n", power);
            gpio_set(&PCH_PWROK_EC, power);

            // Wait for PLT_RST#
            for (;;) {
                bool value = gpio_get(&BUF_PLT_RST_N);
                TRACE("BUF_PLT_RST_N: %d\n", value);
                if (value) break;
                delay_ms(1);
            }

            // enable pnp devices
            pnp_enable();

        } else {
            DEBUG("Disabling power\n");

            // De-assert SUS_ACK#
            TRACE("SUS_PWR_ACK: %d\n", power);
            gpio_set(&SUS_PWR_ACK, power);

            // De-assert PCH_PWROK_EC, SYS_PWROK will be de-asserted
            TRACE("PCH_PWROK_EC: %d\n", power);
            gpio_set(&PCH_PWROK_EC, power);

            // De-assert PM_PWROK, PCH_PWROK will be de-asserted
            TRACE("PM_PWROK: %d\n", power);
            gpio_set(&PM_PWROK, power);

            // De-assert VR_ON
            TRACE("VR_ON: %d\n", power);
            gpio_set(&VR_ON, power);

            // Block processor from controlling SUSB# and SUSC#
            TRACE("EC_EN: %d\n", power);
            gpio_set(&EC_EN, power);

            // De-assert RSMRST#
            TRACE("EC_RSMRST_N: %d\n", power);
            gpio_set(&EC_RSMRST_N, power);

            // Disable VDD5
            TRACE("DD_ON: %d\n", power);
            gpio_set(&DD_ON, power);

            // Wait a minimum of 400 ns (tPCH12)
            delay_ms(1);

            // Disable VCCPRIM_* planes
            TRACE("VA_EC_EN: %d\n", power);
            gpio_set(&VA_EC_EN, power);

            // De-assert DSW_PWROK
            TRACE("PCH_DPWROK_EC: %d\n", power);
            gpio_set(&PCH_DPWROK_EC, power);

            // Wait a minimum of 400 ns (tPCH14)
            delay_ms(1);
        }

        // Reset main loop cycle to force reading PECI and battery
        main_cycle = 0;

        TRACE("LED_PWR: %d\n", power);
        gpio_set(&LED_PWR, power);
    }
    #if LEVEL >= LEVEL_DEBUG
        else if (new && !last) {
            DEBUG("Power switch release\n");

            TRACE("SUSWARN_N: %d\n", gpio_get(&SUSWARN_N));
            TRACE("SUSC_N_PCH: %d\n", gpio_get(&SUSC_N_PCH));
            TRACE("SUSB_N_PCH: %d\n", gpio_get(&SUSB_N_PCH));
            TRACE("ALL_SYS_PWRGD: %d\n", gpio_get(&ALL_SYS_PWRGD));
            TRACE("BUF_PLT_RST_N: %d\n", gpio_get(&BUF_PLT_RST_N));
        }
    #endif

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
        // Checks for power button press
        power_button();
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
