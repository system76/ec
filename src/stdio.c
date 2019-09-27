#include <stdio.h>

#include "include/delay.h"
#include "include/kbscan.h"

void parallel_write(unsigned char value) {
    // Make sure clock is high
    KSOH1 = 0xFF;
    delay_ms(1);

    // Set value
    KSOL = value;
    delay_ms(1);

    // Set clock low
    KSOH1 = 0;
    delay_ms(1);

    // Set clock high again
    KSOH1 = 0xFF;
}

int putchar(int c) {
    unsigned char byte = (unsigned char)c;
    parallel_write(byte);
    return (int)byte;
}
