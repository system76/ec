#include <stdio.h>

#include <ec/i2c.h>

int putchar(int c) {
    unsigned char byte = (unsigned char)c;
    i2c_write(0x76, &byte, 1);
    return (int)byte;
}
