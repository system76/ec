// SPDX-License-Identifier: GPL-3.0-only

#include <app/acpi.h>
#include <app/battery.h>
#include <app/fan.h>
#include <app/kbled.h>
#include <app/lid.h>
#include <app/pwm.h>
#include <board/gpio.h>
#include <common/debug.h>
#include <common/macro.h>
#include <drivers/dgpu/dgpu.h>

#if CONFIG_PLATFORM_INTEL
#include <app/peci.h>
#endif

#ifndef HAVE_LED_AIRPLANE_N
#define HAVE_LED_AIRPLANE_N 1
#endif // HAVE_LED_AIRPLANE_N

extern uint8_t sci_extra;

enum EcOs acpi_ecos = EC_OS_NONE;

static uint8_t fcmd = 0;
static uint8_t fdat = 0;
static uint8_t fbuf[4] = { 0, 0, 0, 0 };

void fcommand(void) {
    switch (fcmd) {
#if CONFIG_HAVE_KBLED
    // Keyboard backlight
    case 0xCA:
        switch (fdat) {
        // Set brightness
        case 0:
            kbled_set(fbuf[0]);
            break;
        // Get brightness
        case 1:
            fbuf[0] = kbled_get();
            break;
        // Get type
        case 2:
            fbuf[0] = kbled_kind;
            break;
        // Set color
        case 3:
            kbled_set_color(
                ((uint32_t)fbuf[0]) |
                ((uint32_t)fbuf[1] << 16) |
                ((uint32_t)fbuf[2] << 8)
            );
            break;
        // Get color
        case 4:
            // uncrustify:off
            {
                uint32_t color = kbled_get_color();
                fbuf[0] = color & 0xFF;
                fbuf[1] = (color >> 16) & 0xFF;
                fbuf[2] = (color >> 8) & 0xFF;
            }
            // uncrustify:on
            break;
        // DUPLICATE: Set brightness
        case 6:
            kbled_set(fbuf[0]);
            break;
        }
        break;
#endif // CONFIG_HAVE_KBLED
    }
}

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

// uncrustify:off
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

#if CONFIG_PLATFORM_INTEL
        ACPI_8(0x07, peci_temp);
#endif

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
            break;

        case 0xBC:
            data = battery_get_start_threshold();
            break;

        case 0xBD:
            data = battery_get_end_threshold();
            break;

        ACPI_8(0xCC, sci_extra);

        ACPI_8(0xCE, fan1_pwm_actual);
        ACPI_16(0xD0, fan1_rpm);
#if CONFIG_HAVE_DGPU
        ACPI_8(0xCD, dgpu_temp);
#endif // CONFIG_HAVE_DGPU
#ifdef FAN2_PWM
        ACPI_8(0xCF, fan2_pwm_actual);
        ACPI_16(0xD2, fan2_rpm);
#endif // FAN2_PWM

        case 0xD4:
            data = fan_get_mode();
            break;

#if HAVE_LED_AIRPLANE_N
        // Airplane mode LED
        case 0xD9:
            if (!gpio_get(&LED_AIRPLANE_N)) {
                data |= BIT(6);
            }
            break;
#endif // HAVE_LED_AIRPLANE_N

        // Set size of flash (from old firmware)
        ACPI_8(0xE5, 0x80);

        ACPI_8(0xF8, fcmd);
        ACPI_8(0xF9, fdat);
        ACPI_8(0xFA, fbuf[0]);
        ACPI_8(0xFB, fbuf[1]);
        ACPI_8(0xFC, fbuf[2]);
        ACPI_8(0xFD, fbuf[3]);
    }

    TRACE("acpi_read %02X = %02X\n", addr, data);
    return data;
}
// uncrustify:on

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
        (void)battery_save_thresholds();
        break;

    case 0xCE:
        if (fan_get_mode() == FAN_MODE_PWM) {
            fan1_pwm_target = data;
        }
        break;

#ifdef FAN2_PWM
    case 0xCF:
        if (fan_get_mode() == FAN_MODE_PWM) {
            fan2_pwm_target = data;
        }
        break;
#endif

    case 0xD4:
        switch (data) {
        case FAN_MODE_AUTO:
        case FAN_MODE_PWM:
            fan_set_mode(data);
            break;
        }
        break;

#if HAVE_LED_AIRPLANE_N
    // Airplane mode LED
    case 0xD9:
        gpio_set(&LED_AIRPLANE_N, !(bool)(data & BIT(6)));
        break;
#endif

    case 0xF8:
        fcmd = data;
        fcommand();
        break;
    case 0xF9:
        fdat = data;
        break;
    case 0xFA:
        fbuf[0] = data;
        break;
    case 0xFB:
        fbuf[1] = data;
        break;
    case 0xFC:
        fbuf[2] = data;
        break;
    case 0xFD:
        fbuf[3] = data;
        break;
    }
}
