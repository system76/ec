#include <8051.h>

#include <arch/delay.h>
#include <arch/timer.h>

void delay_ticks(uint16_t ticks) {
    timer_mode_1(-ticks);
    timer_wait();
    timer_stop();
}

// 1 us * 9.2 MHz / 12 is 69/90
// Warning: this will round to the nearest tick
#define delay_us(X) \
    delay_ticks((uint16_t)((((uint32_t)(X)) * 69UL + 89UL) / 90UL));

// 1 ns * 9.2 MHz / 12 is 69/90000
// Warning: this will round to the nearest tick
#define delay_ns(X) \
    delay_ticks((uint16_t)((((uint32_t)(X)) * 69UL + 89999UL) / 90000UL));

// This loops through delays of one ms in order to avoid overflow
void delay_ms(int ms) {
    for (int i = 0; i < ms; i++) {
        delay_us(1000);
    }
}
