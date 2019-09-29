#include <stdio.h>

#include "include/acpi.h"

uint8_t acpi_read(uint8_t addr) {
    uint8_t data = 0;
    //TODO
    printf("acpi_read %02X = %02X\n", addr, data);
    return data;
}

void acpi_write(uint8_t addr, uint8_t data) {
    //TODO
    printf("acpi_write %02X = %02X\n", addr, data);
}
