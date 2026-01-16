// SPDX-License-Identifier: GPL-3.0-only

#include <app/battery.h>
#include <app/board.h>
#include <app/dgpu.h>
#include <app/ecpm.h>
#include <app/fan.h>
#include <app/gctrl.h>
#include <app/kbc.h>
#include <app/kbled.h>
#include <app/kbscan.h>
#include <app/lid.h>
#include <app/pmc.h>
#include <app/power.h>
#include <app/ps2.h>
#include <app/pwm.h>
#include <app/smbus.h>
#include <app/smfi.h>
#include <app/usbpd.h>
#include <arch/arch.h>
#include <arch/delay.h>
#include <arch/time.h>
#include <board/gpio.h>
#include <board/keymap.h>
#include <common/debug.h>
#include <common/macro.h>
#include <common/version.h>
#include <ec/ec.h>

#if CONFIG_PLATFORM_INTEL
#include <app/peci.h>
#endif

#ifdef PARALLEL_DEBUG
#include <app/parallel.h>
#endif // PARALLEL_DEBUG

#include <8051.h>
#include <stdio.h>

void external_0(void) __interrupt(0) {}
// timer_0 is in time.c
void timer_0(void) __interrupt(1);
void external_1(void) __interrupt(2) {}
void timer_1(void) __interrupt(3) {}
void serial(void) __interrupt(4) {}
void timer_2(void) __interrupt(5) {}

uint8_t main_cycle = 0;

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

    systick_t last_time_5ms = 0;
    systick_t last_time_100ms = 0;
    systick_t last_time_250ms = 0;
    systick_t last_time_500ms = 0;
    systick_t last_time_1sec = 0;

    __bit evt_1ms = 0;
    __bit evt_5ms = 0;
    __bit evt_100ms = 0;
    __bit evt_250ms = 0;
    __bit evt_500ms = 0;
    __bit evt_1sec = 0;

    for (main_cycle = 0;; main_cycle++) {
        // Calculate which intervals need to run at systick update.
        if (evt_systick) {
            evt_systick = 0;

            systick_t time = time_get();

            evt_1ms = 1;

            if ((time - last_time_5ms) >= INTERVAL_5MS) {
                last_time_5ms = time;
                evt_5ms = 1;
            }
            if ((time - last_time_100ms) >= INTERVAL_100MS) {
                last_time_100ms = time;
                evt_100ms = 1;
            }
            if ((time - last_time_250ms) >= INTERVAL_250MS) {
                last_time_250ms = time;
                evt_250ms = 1;
            }
            if ((time - last_time_500ms) >= INTERVAL_500MS) {
                last_time_500ms = time;
                evt_500ms = 1;
            }
            if ((time - last_time_1sec) >= INTERVAL_1SEC) {
                last_time_1sec = time;
                evt_1sec = 1;
            }
        }

        if (evt_1ms) {
            evt_1ms = 0;

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

        if (evt_5ms) {
            evt_5ms = 0;

#if PARALLEL_DEBUG
            if (!parallel_debug)
#endif // PARALLEL_DEBUG
            {
                // Scans keyboard and sends keyboard packets
                kbscan_event();
            }
        }

        if (evt_100ms) {
            evt_100ms = 0;

            fan_update_duty();
        }

        if (evt_250ms) {
            evt_250ms = 0;

#if CONFIG_PLATFORM_INTEL
            peci_read_temp();
#endif
            dgpu_read_temp();
        }

        if (evt_500ms) {
            evt_500ms = 0;

            // Handle lid close/open
            lid_event();
        }

        if (evt_1sec) {
            evt_1sec = 0;

            battery_event();

            if (fan_get_mode() == FAN_MODE_AUTO) {
                fan_update_target();
            }
        }

        // Idle until next timer interrupt
        //PCON |= BIT(0);
    }
}
