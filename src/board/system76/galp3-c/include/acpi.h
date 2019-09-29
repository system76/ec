#ifndef _ACPI_H_
#define _ACPI_H_

#include <stdint.h>

uint8_t acpi_read(uint8_t addr);
void acpi_write(uint8_t addr, uint8_t data);

#endif // _ACPI_H_
