// SPDX-License-Identifier: GPL-3.0-only

#ifndef _COMMON_I2C_H
#define _COMMON_I2C_H

#include <stdbool.h>
#include <stdint.h>

// I2C bus, should be defined elsewhere
struct I2C;

// Start i2c transaction
// Must be defined by arch, board, or ec
int i2c_start(struct I2C * i2c, uint8_t addr, bool read);

// Stop i2c transaction
// Must be defined by arch, board, or ec
void i2c_stop(struct I2C * i2c);

// Send a byte on i2c bus
// Must be defined by arch, board, or ec
int i2c_write(struct I2C * i2c, uint8_t * data, int length);

// Read bytes from bus
// Must be defined by arch, board, or ec
int i2c_read(struct I2C * i2c, uint8_t * data, int length);

// Read multiple bytes from address in one transaction
int i2c_recv(struct I2C * i2c, uint8_t addr, uint8_t* data, int length);

// Write multiple bytes to address in one transaction
int i2c_send(struct I2C * i2c, uint8_t addr, uint8_t* data, int length);

// Read multiple bytes from a register in one transaction
int i2c_get(struct I2C * i2c, uint8_t addr, uint8_t reg, uint8_t* data, int length);

// Write multiple bytes to a register in one transaction
int i2c_set(struct I2C * i2c, uint8_t addr, uint8_t reg, uint8_t* data, int length);

#endif // _COMMON_I2C_H
