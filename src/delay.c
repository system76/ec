#include <8051.h>

#include "include/delay.h"

void timer_clear(void) {
    TR0 = 0;
    TF0 = 0;
}

void timer_mode_1(int value) {
    timer_clear();
    TMOD = 0x01;
    TH0 = (unsigned char)(value >> 8);
    TL0 = (unsigned char)value;
    TR0 = 1;
}

void delay_ms(int ms) {
    for (int i = 0; i < ms; i++) {
        // One millisecond in ticks is determined as follows:
        //   9.2 MHz is the clock rate
        //   The timer divider is 12
        //   The timer rate is 12 / 9.2 MHz = 1.304 us
        //   The ticks are 1000 ms / (1.304 us) = 766.667
        //   65536 - 766.667 = 64769.33
        timer_mode_1(64769);
        while (TF0 == 0) {}
        timer_clear();
    }
}
