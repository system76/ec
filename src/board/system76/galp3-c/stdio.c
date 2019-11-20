#include <stdio.h>
#include <mcs51/8051.h>

#ifdef I2C_DEBUGGER
    #include <common/i2c.h>
#endif

int putchar(int c) {
    unsigned char byte = (unsigned char)c;
    SBUF = byte;
#ifdef I2C_DEBUGGER
    i2c_send(I2C_DEBUGGER, &byte, 1);
#endif
    return (int)byte;
}
