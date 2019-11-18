#include <stdio.h>

#include <common/i2c.h>

int putchar(int c) {
    unsigned char byte = (unsigned char)c;
#ifdef I2C_DEBUGGER
    i2c_send(I2C_DEBUGGER, &byte, 1);
#endif
    return (int)byte;
}
