#include <8051.h>

#include <arch/timer.h>

void timer_mode_1(int value) {
    timer_stop();
    TMOD = 0x01;
    TH0 = (unsigned char)(value >> 8);
    TL0 = (unsigned char)value;
    TR0 = 1;
}

void timer_wait(void) {
    while (TF0 == 0) {}
}

void timer_stop(void) {
    TR0 = 0;
    TF0 = 0;
}
