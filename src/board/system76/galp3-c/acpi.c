#include <board/acpi.h>
#include <board/battery.h>
#include <board/peci.h>
#include <common/debug.h>
#include <ec/gpio.h>

static struct Gpio __code ACIN_N = GPIO(B, 6);
static struct Gpio __code LID_SW_N = GPIO(D, 1);

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

        // Bypass status test in ACPI - TODO
        case 0x26:
            data |= 1 << 1;
            break;

        ACPI_16(0x2A, battery_current);
        ACPI_16(0x2E, battery_remaining_capacity);
        ACPI_16(0x32, battery_voltage);

        // Set size of flash (from old firmware)
        ACPI_8 (0xE5, 0x80);
    }

    DEBUG("acpi_read %02X = %02X\n", addr, data);
    return data;
}


// If not in debug mode, data is not used, ignore warning
#pragma save
#pragma disable_warning 85
void acpi_write(uint8_t addr, uint8_t data) {
    DEBUG("acpi_write %02X = %02X\n", addr, data);

    switch (addr) {
    //TODO
    }
}
#pragma restore
