// SPDX-License-Identifier: GPL-3.0-only

#include <avr/io.h>

#include <board/cpu.h>
#include <board/i2c.h>

void i2c_init(uint32_t baud) {
    TWAR = 0;
    TWBR = (uint8_t)(((F_CPU / baud) - 16) / 2);
    TWCR = 0;
}
