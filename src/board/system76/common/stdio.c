// SPDX-License-Identifier: GPL-3.0-only

#include <stdio.h>

#include <board/smfi.h>

#ifdef SERIAL_DEBUGGER
    #include <mcs51/8051.h>
#endif

#ifdef I2C_DEBUGGER
    #include <ec/i2c.h>
#endif

#ifdef PARALLEL_DEBUG
    #include <board/parallel.h>
#endif // PARALLEL_DEBUG

// Disables interrupts so it can be called from interrupt handler
int putchar(int c) __critical {
    unsigned char byte = (unsigned char)c;

    smfi_debug(byte);

#ifdef SERIAL_DEBUGGER
    SBUF = byte;
#endif

#ifdef I2C_DEBUGGER
    i2c_send(&I2C_SMBUS, I2C_DEBUGGER, &byte, 1);
#endif

#ifdef PARALLEL_DEBUG
    if (parallel_debug) {
        parallel_write(&byte, 1);
    }
#endif // PARALLEL_DEBUG

    return (int)byte;
}
