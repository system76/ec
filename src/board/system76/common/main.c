// SPDX-License-Identifier: GPL-3.0-only

#include <8051.h>
#include <stdio.h>

#include <arch/arch.h>
#include <arch/delay.h>
#include <arch/time.h>
#include <board/battery.h>
#include <board/board.h>
#include <board/dgpu.h>
#include <board/ecpm.h>
#include <board/gpio.h>
#include <board/gctrl.h>
#include <board/kbc.h>
#include <board/kbled.h>
#include <board/kbscan.h>
#include <board/keymap.h>
#include <board/lid.h>
#include <board/peci.h>
#include <board/pmc.h>
#include <board/power.h>
#include <board/ps2.h>
#include <board/pwm.h>
#include <board/smbus.h>
#include <board/smfi.h>
#include <common/debug.h>
#include <common/macro.h>
#include <common/version.h>
#include <ec/ec.h>

#ifdef PARALLEL_DEBUG
    #include <board/parallel.h>
#endif // PARALLEL_DEBUG

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
    ec_init();
    gctrl_init();
    gpio_init();

    // Can happen in any order
#if HAVE_DGPU
    dgpu_init();
#endif
    ecpm_init();
    kbc_init();
    kbled_init();
#ifdef PARALLEL_DEBUG
    parallel_debug = false;
    if (parallel_init()) {
        parallel_debug = true;
    } else
#endif // PARALLEL_DEBUG
    {
        kbscan_init();
    }
    keymap_init();
    peci_init();
    pmc_init();
    pwm_init();
    smbus_init();
    smfi_init();

    //TODO: INTC

    // Must happen last
    board_init();
}

void main(void) {
    init();

    INFO("\n");

#if GPIO_DEBUG
    gpio_debug();
#endif

    INFO("System76 EC board '%s', version '%s'\n", board(), version());

    uint32_t last_time = 0;
    for(main_cycle = 0; ; main_cycle++) {
        switch (main_cycle % 3) {
            case 0:
                // Handle power states
                power_event();
                break;
            case 1:
#if PARALLEL_DEBUG
                if (!parallel_debug)
#endif // PARALLEL_DEBUG
                {
                    // Scans keyboard and sends keyboard packets
                    kbscan_event();
                }
                break;
            case 2:
                // Handle lid close/open
                lid_event();
                break;
        }

        if (main_cycle == 0) {
            uint32_t time = time_get();
            // Only run the following once a second
            if (last_time > time || (time - last_time) >= 1000) {
                last_time = time;

                // Updates fan status and temps
                peci_event();

#if HAVE_DGPU
                // Updates discrete GPU fan status and temps
                dgpu_event();
#endif

                // Updates battery status
                battery_event();
            }
        }

        // Board-specific events
        board_event();

        // Checks for keyboard/mouse packets from host
        kbc_event(&KBC);
        // Handles ACPI communication
        pmc_event(&PMC_1);
        // AP/EC communication over SMFI
        smfi_event();
        // Idle until next timer interrupt
        //Disabled until interrupts used: PCON |= 1;
    }
}
