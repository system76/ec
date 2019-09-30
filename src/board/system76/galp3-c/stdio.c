#include <stdio.h>

#include <arch/timer.h>
#include <board/kbscan.h>

// Wait 25 us
// 65536 - (25 / 1.304) = 65517
void parallel_delay(void) {
    timer_mode_1(65517);
    timer_wait();
    timer_stop();
}

// This takes a time of 25 us * 3 = 75 us
// That produces a frequency of 13.333 KHz
// Which produces a bitrate of 106.667 KHz
void parallel_write(unsigned char value) {
    // Make sure clock is high
    KSOH1 = 0xFF;
    parallel_delay();

    // Set value
    KSOL = value;
    parallel_delay();

    // Set clock low
    KSOH1 = 0;
    parallel_delay();

    // Set clock high again
    KSOH1 = 0xFF;
}

int putchar(int c) {
    unsigned char byte = (unsigned char)c;
    parallel_write(byte);
    return (int)byte;
}
