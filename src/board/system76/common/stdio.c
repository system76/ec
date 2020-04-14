#include <stdio.h>

#include <board/smfi.h>

#ifdef SERIAL_DEBUGGER
    #include <mcs51/8051.h>
#endif

#ifdef I2C_DEBUGGER
    #include <ec/i2c.h>
#endif

#ifdef PARPORT_DEBUG
    #include <ec/parallel.h>
#endif

int putchar(int c) {
    unsigned char byte = (unsigned char)c;
    smfi_debug(byte);
#ifdef SERIAL_DEBUGGER
    SBUF = byte;
#endif
#ifdef I2C_DEBUGGER
    i2c_send(&I2C_SMBUS, I2C_DEBUGGER, &byte, 1);
#endif
#ifdef PARPORT_DEBUG
    parport_write(&byte, 1);
#endif
    return (int)byte;
}
