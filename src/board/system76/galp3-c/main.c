#include <8051.h>
#include <stdio.h>

#include <arch/arch.h>
#include <arch/delay.h>
#include <arch/time.h>
#include <board/battery.h>
#include <board/ecpm.h>
#include <board/gpio.h>
#include <board/gctrl.h>
#include <board/kbc.h>
#include <board/kbled.h>
#include <board/kbscan.h>
#include <board/lid.h>
#include <board/peci.h>
#include <board/pmc.h>
#include <board/power.h>
#include <board/ps2.h>
#include <board/pwm.h>
#include <board/smbus.h>
#include <common/debug.h>
#include <common/macro.h>


void external_0(void) __interrupt(0) {}
// timer_0 is in time.c
void timer_0(void) __interrupt(1);
void external_1(void) __interrupt(2) {}
void timer_1(void) __interrupt(3) {}
void serial(void) __interrupt(4) {}
void timer_2(void) __interrupt(5) {}

uint8_t main_cycle = 0;

void init(void) {
    // Must happen first
    arch_init();
    gctrl_init();
    gpio_init();

    // Can happen in any order
    ecpm_init();
    kbc_init();
    kbled_init();
    kbscan_init();
    peci_init();
    pmc_init();
    pwm_init();
    smbus_init();

    //TODO: INTC
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

void main(void) {
    init();

    INFO("\n");

#if GPIO_DEBUG
    gpio_debug();
#endif

    // Allow CPU to boot
    gpio_set(&SB_KBCRST_N, true);
    // Allow backlight to be turned on
    gpio_set(&BKL_EN, true);
    // Enable camera
    gpio_set(&CCD_EN, true);
    // Enable wireless
    gpio_set(&BT_EN, true);
    gpio_set(&WLAN_EN, true);
    gpio_set(&WLAN_PWR_EN, true);
    // Enable right USB port
    gpio_set(&USB_PWR_EN_N, false);
    // Assert SMI#, SCI#, and SWI#
    gpio_set(&SCI_N, true);
    gpio_set(&SMI_N, true);
    gpio_set(&SWI_N, true);

    INFO("Hello from System76 EC for %s!\n", xstr(__BOARD__));

    for(main_cycle = 0; ; main_cycle++) {
        // Handle power states
        power_event();
        // Scans keyboard and sends keyboard packets
        kbscan_event();
        // Passes through touchpad packets
        touchpad_event(&PS2_3);
        // Handle lid close/open
        lid_event();
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
        // Idle until next timer interrupt
        PCON |= 1;
    }
}
