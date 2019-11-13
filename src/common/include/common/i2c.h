#ifndef _COMMON_I2C_H
#define _COMMON_I2C_H

#include <stdbool.h>
#include <stdint.h>

// Start i2c transaction
// Must be defined by arch, board, or ec
int i2c_start(uint8_t addr, bool read);

// Stop i2c transaction
// Must be defined by arch, board, or ec
void i2c_stop(void);

// Send a byte on i2c bus
// Must be defined by arch, board, or ec
int i2c_write(uint8_t * data, int length);

// Read bytes from bus
// Must be defined by arch, board, or ec
int i2c_read(uint8_t * data, int length);

// Read multiple bytes from address in one transaction
int i2c_recv(uint8_t addr, uint8_t* data, int length);

// Write multiple bytes to address in one transaction
int i2c_send(uint8_t addr, uint8_t* data, int length);

// Read multiple bytes from a register in one transaction
int i2c_get(uint8_t addr, uint8_t reg, uint8_t* data, int length);

// Write multiple bytes to a register in one transaction
int i2c_set(uint8_t addr, uint8_t reg, uint8_t* data, int length);

#endif // _COMMON_I2C_H
