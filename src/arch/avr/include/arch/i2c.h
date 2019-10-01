#ifndef _ARCH_I2C_H
#define _ARCH_I2C_H

uint8_t i2c_recv(uint8_t addr, uint8_t* data, uint16_t length);
uint8_t i2c_send(uint8_t addr, uint8_t* data, uint16_t length);

uint8_t i2c_get(uint8_t addr, uint8_t reg, uint8_t* data, uint16_t length);
uint8_t i2c_set(uint8_t addr, uint8_t reg, uint8_t* data, uint16_t length);

#endif // _ARCH_I2C_H
