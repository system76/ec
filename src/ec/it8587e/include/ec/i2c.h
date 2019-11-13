#ifndef _EC_I2C_H
#define _EC_I2C_H

#include <stdint.h>

uint8_t i2c_transaction(uint8_t addr_rw, uint8_t * data, uint16_t length);
#define i2c_read(addr, data, length) i2c_transaction(((uint8_t)addr << 1) | 1, data, length)
#define i2c_write(addr, data, length) i2c_transaction(((uint8_t)addr << 1), data, length)

#endif // _EC_I2C_H
