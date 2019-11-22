#include <8051.h>

#include <arch/delay.h>
#include <arch/timer.h>

void delay_ticks(uint16_t ticks) {
    timer_mode_1(-ticks);
    timer_wait();
    timer_stop();
}

// This loops through delays of one ms in order to avoid overflow
void delay_ms(int ms) {
    for (int i = 0; i < ms; i++) {
        delay_us(1000);
    }
}
