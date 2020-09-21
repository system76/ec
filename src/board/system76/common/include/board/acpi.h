// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_ACPI_H
#define _BOARD_ACPI_H

#include <stdint.h>

enum EcOs {
    // No ACPI or driver support
    EC_OS_NONE=0,
    // ACPI, but no driver support
    EC_OS_ACPI=1,
    // ACPI with driver, full support
    EC_OS_FULL=2,
};
extern enum EcOs acpi_ecos;

void acpi_reset(void);
uint8_t acpi_read(uint8_t addr);
void acpi_write(uint8_t addr, uint8_t data);

#endif // _BOARD_ACPI_H
