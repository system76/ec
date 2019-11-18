#include <stdio.h>

#include <common/i2c.h>

#define I2C_DEBUGGER 0
// #define I2C_DEBUGGER 0x76

int putchar(int c) {
    unsigned char byte = (unsigned char)c;
#if I2C_DEBUGGER
    i2c_send(I2C_DEBUGGER, &byte, 1);
#endif
    return (int)byte;
}
