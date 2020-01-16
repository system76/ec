#ifndef _BOARD_ACPI_H
#define _BOARD_ACPI_H

#include <stdint.h>

uint8_t acpi_read(uint8_t addr);
void acpi_write(uint8_t addr, uint8_t data);

#endif // _BOARD_ACPI_H
