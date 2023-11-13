// SPDX-License-Identifier: GPL-3.0-only

#include <board/acpi.h>
#include <board/battery.h>
#include <board/dgpu.h>
#include <board/gpio.h>
#include <board/kbled.h>
#include <board/lid.h>
#include <board/peci.h>
#include <board/power.h>
#include <common/macro.h>
#include <common/debug.h>
#include <ec/pwm.h>

#ifndef HAVE_LED_AIRPLANE_N
#define HAVE_LED_AIRPLANE_N 1
#endif // HAVE_LED_AIRPLANE_N

extern uint8_t sci_extra;

enum EcOs acpi_ecos = EC_OS_NONE;

extern bool pmc_s0_hack;

void acpi_reset(void) {
    // Disable lid wake
    lid_wake = false;

    // ECOS: No ACPI or driver
    acpi_ecos = EC_OS_NONE;

#if HAVE_LED_AIRPLANE_N
    // Clear airplane mode LED
    gpio_set(&LED_AIRPLANE_N, true);
#endif
}

// clang-format off
uint8_t acpi_read(uint8_t addr) {
    uint8_t data = 0;

    #define ACPI_8(K, V) \
    case (K): \
        data = (uint8_t)(V); \
        break

    #define ACPI_16(K, V) \
        ACPI_8(K, V); \
        ACPI_8((K) + 1, (V) >> 8)

    #define ACPI_32(K, V) \
        ACPI_16(K, V); \
        ACPI_16((K) + 2, (V) >> 16)

    switch (addr) {
        // Lid state and other flags
        case 0x03:
            if (gpio_get(&LID_SW_N)) {
                // Lid is open
                data |= BIT(0);
            }
            if (lid_wake) {
                data |= BIT(2);
            }
            break;

        ACPI_8(0x07, peci_temp);

        // Handle AC adapter and battery present
        case 0x10:
            if (!gpio_get(&ACIN_N)) {
                // AC adapter connected
                data |= BIT(0);
            }
            if (battery_info.status & BATTERY_INITIALIZED) {
                // BAT0 connected
                data |= BIT(2);
            }
            break;

        ACPI_16(0x16, battery_info.design_capacity);
        ACPI_16(0x1A, battery_info.full_capacity);
        ACPI_16(0x22, battery_info.design_voltage);

        case 0x26:
            // If AC adapter connected
            if (!gpio_get(&ACIN_N)) {
                // And battery is not fully charged
                if (battery_info.current != 0) {
                    // Battery is charging
                    data |= BIT(1);
                }
            }
            break;

        ACPI_16(0x2A, battery_info.current);
        ACPI_16(0x2E, battery_info.remaining_capacity);
        ACPI_16(0x32, battery_info.voltage);

        ACPI_16(0x42, battery_info.cycle_count);

        case 0x68:
            data = acpi_ecos;
            // HACK: Kick PMC to fix suspend on lemp11
            pmc_s0_hack = true;
            break;

        case 0xBC:
            data = battery_get_start_threshold();
            break;

        case 0xBD:
            data = battery_get_end_threshold();
            break;

        ACPI_8(0xCC, sci_extra);

        ACPI_8(0xCE, DCR2);
        ACPI_8(0xD0, F1TLRR);
        ACPI_8(0xD1, F1TMRR);
#if HAVE_DGPU
        ACPI_8(0xCD, dgpu_temp);
        ACPI_8(0xCF, DCR4);
        ACPI_8(0xD2, F2TLRR);
        ACPI_8(0xD3, F2TMRR);
#endif // HAVE_DGPU

#if HAVE_LED_AIRPLANE_N
        // Airplane mode LED
        case 0xD9:
            if (!gpio_get(&LED_AIRPLANE_N)) {
                data |= BIT(6);
            }
            break;
#endif // HAVE_LED_AIRPLANE_N

        // S0ix hook
        case 0xE0:
            data = pep_in_s0ix;
            break;
        case 0xE1:
            data = pep_display_on;
            break;
    }

    TRACE("acpi_read %02X = %02X\n", addr, data);
    return data;
}
// clang-format on

void acpi_write(uint8_t addr, uint8_t data) {
    TRACE("acpi_write %02X = %02X\n", addr, data);

    switch (addr) {
    // Lid state and other flags
    case 0x03:
        lid_wake = (bool)(data & BIT(2));
        break;

    case 0x68:
        acpi_ecos = (enum EcOs)data;
        break;

    case 0xBC:
        battery_set_start_threshold(data);
        break;

    case 0xBD:
        battery_set_end_threshold(data);
        break;

#if HAVE_LED_AIRPLANE_N
    // Airplane mode LED
    case 0xD9:
        gpio_set(&LED_AIRPLANE_N, !(bool)(data & BIT(6)));
        break;
#endif
    // S0ix hook
    case 0xE0:
        pep_in_s0ix = !!data;
        break;
    // Display hook
    case 0xE1:
        pep_display_on = !!data;
        break;
    }
}
