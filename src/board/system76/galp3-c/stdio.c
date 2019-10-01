#include <stdio.h>

#include <ec/smbus.h>

void i2c_write(unsigned char value) {
    for (;;) {
        // Wait for last command
        while (HOSTAA & 1) {}

        // Clear result
        HOSTAA = HOSTAA;

        // Clock down to 50 KHz
        SCLKTSA = 1;

        // Enable host interface with i2c compatibility
        HOCTL2A = (1 << 1) | (1 << 0);

        // Write value to 0x76
        TRASLAA = (0x76 << 1) | (0 << 1);
        HOCMDA = value;

        // Start command
        HOCTLA = (1 << 6) | (0b001 << 2);

        // Wait for command to start
        while (!(HOSTAA & 1)) {}

        // Wait for command to finish
        while (HOSTAA & 1) {}

        // Read and clear status
        uint8_t status = HOSTAA;
        HOSTAA = status;

        // If there were no errors, return
        uint8_t error = (1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2);
        if (!(status & error)) {
            break;
        }
    }
}

int putchar(int c) {
    unsigned char byte = (unsigned char)c;
    i2c_write(byte);
    return (int)byte;
}
