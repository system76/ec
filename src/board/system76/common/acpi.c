#include <board/acpi.h>
#include <board/battery.h>
#include <board/gpio.h>
#include <board/kbled.h>
#include <board/lid.h>
#include <board/peci.h>
#include <common/debug.h>

#ifndef HAVE_LED_AIRPLANE_N
#define HAVE_LED_AIRPLANE_N 1
#endif // HAVE_LED_AIRPLANE_N

extern uint8_t sci_extra;

uint8_t acpi_ecos = 0;

static uint8_t fcmd = 0;
static uint8_t fdat = 0;
static uint8_t fbuf[4] = { 0, 0, 0, 0 };

void fcommand(void) {
    switch (fcmd) {
        // Keyboard backlight
        case 0xCA:
            switch (fdat) {
                // Set white LED brightness
                case 0x00:
                    kbled_set(fbuf[0]);
                    break;
                // Get white LED brightness
                case 0x01:
                    fbuf[0] = kbled_get();
                    break;
                // Set LED color
                case 0x03:
                    kbled_set_color(
                        ((uint32_t)fbuf[0]) |
                        ((uint32_t)fbuf[1] << 16) |
                        ((uint32_t)fbuf[2] << 8)
                    );
                    break;
                // Set LED brightness
                case 0x06:
                    kbled_set(fbuf[0]);
                    break;
            }
            break;
    }
}

void acpi_reset(void) {
    // Disable lid wake
    lid_wake = false;

    // ECOS: No ACPI
    acpi_ecos = 0;

#if HAVE_LED_AIRPLANE_N
    // Clear airplane mode LED
    gpio_set(&LED_AIRPLANE_N, true);
#endif
}

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
                data |= 1 << 0;
            }
            if (lid_wake) {
                data |= 1 << 2;
            }
            break;

        // Handle AC adapter and battery present
        case 0x10:
            if (!gpio_get(&ACIN_N)) {
                // AC adapter connected
                data |= 1 << 0;
            }
            // BAT0 always connected - TODO
            data |= 1 << 2;
            break;

        ACPI_16(0x16, battery_design_capacity);
        ACPI_16(0x1A, battery_full_capacity);
        ACPI_16(0x22, battery_design_voltage);

        case 0x26:
            // If AC adapter connected
            if (!gpio_get(&ACIN_N)) {
                // And battery is not fully charged
                if (battery_current != 0) {
                    // Battery is charging
                    data |= 1 << 1;
                }
            }
            break;

        ACPI_16(0x2A, battery_current);
        ACPI_16(0x2E, battery_remaining_capacity);
        ACPI_16(0x32, battery_voltage);

        ACPI_8(0x68, acpi_ecos);

        ACPI_8(0xCC, sci_extra);

#if HAVE_LED_AIRPLANE_N
        // Airplane mode LED
        case 0xD9:
            if (!gpio_get(&LED_AIRPLANE_N)) {
                data |= (1 << 6);
            }
            break;
#endif // HAVE_LED_AIRPLANE_N

        // Set size of flash (from old firmware)
        ACPI_8 (0xE5, 0x80);

        ACPI_8 (0xF8, fcmd);
        ACPI_8 (0xF9, fdat);
        ACPI_8 (0xFA, fbuf[0]);
        ACPI_8 (0xFB, fbuf[1]);
        ACPI_8 (0xFC, fbuf[2]);
        ACPI_8 (0xFD, fbuf[3]);
    }

    DEBUG("acpi_read %02X = %02X\n", addr, data);
    return data;
}

void acpi_write(uint8_t addr, uint8_t data) {
    DEBUG("acpi_write %02X = %02X\n", addr, data);

    switch (addr) {
        // Lid state and other flags
        case 0x03:
            lid_wake = (bool)(data & (1 << 2));
            break;

        case 0x68:
            acpi_ecos = data;
            break;

#if HAVE_LED_AIRPLANE_N
        // Airplane mode LED
        case 0xD9:
            gpio_set(&LED_AIRPLANE_N, !(bool)(data & (1 << 6)));
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
