// SPDX-License-Identifier: GPL-3.0-only

#include <board/smbus.h>
#include <ec/i2c.h>

void smbus_init(void) {
    // 9.2 MHz * 4.7 us = 43.24
    SMB4P7USL = 43;
    // 9.2 MHz * 4.0 us = 36.8
    SMB4P0USL = 37;
    // 9.2 MHz * 300 ns = 2.76
    SMB300NS = 3;
    // 9.2 MHz * 250 ns = 2.3
    SMB250NS = 2;
    // 1.024 KHz * 25 ms = 25.6
    SMB25MS = 26;
    // 9.2 MHz * 45.3 us = 416.76 (0x01A1)
    SMB45P3USL = 0xA1;
    SMB45P3USH = 0x01;

    // Set up for i2c usage
    i2c_reset(&I2C_SMBUS, true);
}

int smbus_read(uint8_t address, uint8_t command, uint16_t * data) {
    return i2c_get(&I2C_SMBUS, address, command, (uint8_t *)data, 2);
}

int smbus_write(uint8_t address, uint8_t command, uint16_t data) {
    return i2c_set(&I2C_SMBUS, address, command, (uint8_t *)&data, 2);
}
