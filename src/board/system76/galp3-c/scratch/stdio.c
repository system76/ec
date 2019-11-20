#include <mcs51/8051.h>
#include <stdio.h>

int putchar(int c) {
    unsigned char byte = (unsigned char)c;
    SBUF = byte;
    return (int)byte;
}
