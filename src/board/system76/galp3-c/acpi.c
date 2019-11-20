#include <board/acpi.h>
#include <board/battery.h>
#include <board/peci.h>
#include <common/debug.h>

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
        ACPI_16(0x00, battery_temp);
        ACPI_16(0x02, battery_voltage);
        ACPI_16(0x04, battery_current);
        ACPI_16(0x06, battery_charge);

        ACPI_16(0x10, peci_offset);
        ACPI_16(0x12, peci_temp);
        ACPI_8 (0x14, peci_duty);
        ACPI_8 (0x15, peci_tcontrol);
        ACPI_8 (0x16, peci_tjmax);

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
