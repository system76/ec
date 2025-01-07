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
#include <board/fan.h>
#include <board/gpio.h>
#include <board/gctrl.h>
#include <board/kbc.h>
#include <board/kbled.h>
#include <board/kbscan.h>
#include <board/keymap.h>
#include <board/lid.h>
#include <board/pmc.h>
#include <board/power.h>
#include <board/ps2.h>
#include <board/pwm.h>
#include <board/smbus.h>
#include <board/smfi.h>
#include <board/usbpd.h>
#include <common/debug.h>
#include <common/macro.h>
#include <common/version.h>
#include <ec/ec.h>

#if CONFIG_PLATFORM_INTEL
#include <board/peci.h>
#endif

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

#define INTERVAL_1MS    1U
#define INTERVAL_5MS    5U
#define INTERVAL_100MS  100U
#define INTERVAL_250MS  250U
#define INTERVAL_500MS  500U
#define INTERVAL_1SEC   1000U

void init(void) {
    // Must happen first
    arch_init();
    ec_init();
    gctrl_init();
    gpio_init();

    // Can happen in any order
#if CONFIG_HAVE_DGPU
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
#if CONFIG_PLATFORM_INTEL
    peci_init();
#endif
    pmc_init();
    pwm_init();
    smbus_init();
    smfi_init();
    usbpd_init();

    //TODO: INTC

    // Must happen last
    power_init();
    board_init();
    (void)battery_load_thresholds();
}

void main(void) {
    init();

    INFO("\n");

#ifdef GPIO_DEBUG
    gpio_debug();
#endif

    INFO("System76 EC board '%s', version '%s'\n", board(), version());

    systick_t last_time_1ms = 0;
    systick_t last_time_5ms = 0;
    systick_t last_time_100ms = 0;
    systick_t last_time_250ms = 0;
    systick_t last_time_500ms = 0;
    systick_t last_time_1sec = 0;

    for (main_cycle = 0;; main_cycle++) {
        systick_t time = time_get();

        if ((time - last_time_1ms) >= INTERVAL_1MS) {
            last_time_1ms = time;

            // Handle USB-C events immediately before power states
            usbpd_event();
            // Handle power states
            power_event();

            // Board-specific events
            board_event();
            // Checks for keyboard/mouse packets from host
            kbc_event(&KBC);
            // Handles ACPI communication
            pmc_event(&PMC_1);
            // AP/EC communication over SMFI
            smfi_event();
        }

        if ((time - last_time_5ms) >= INTERVAL_5MS) {
            last_time_5ms = time;

#if PARALLEL_DEBUG
            if (!parallel_debug)
#endif // PARALLEL_DEBUG
            {
                // Scans keyboard and sends keyboard packets
                kbscan_event();
            }
        }

        if ((time - last_time_100ms) >= INTERVAL_100MS) {
            last_time_100ms = time;

            fan_update_duty();
        }

        if ((time - last_time_250ms) >= INTERVAL_250MS) {
            last_time_250ms = time;

#if CONFIG_PLATFORM_INTEL
            peci_read_temp();
#endif
            dgpu_read_temp();
        }

        if ((time - last_time_500ms) >= INTERVAL_500MS) {
            last_time_500ms = time;

            // Handle lid close/open
            lid_event();
        }

        if ((time - last_time_1sec) >= INTERVAL_1SEC) {
            last_time_1sec = time;

            battery_event();

            if (fan_get_mode() == FAN_MODE_AUTO) {
                fan_update_target();
            }
        }

        // Idle until next timer interrupt
        //PCON |= BIT(0);
    }
}
