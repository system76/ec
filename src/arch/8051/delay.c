#include <8051.h>

#include <arch/delay.h>
#include <arch/timer.h>

void delay_ticks(uint16_t ticks) {
    timer_mode_1(-ticks);
    timer_wait();
    timer_stop();
}

// One millisecond in ticks is determined as follows:
//   9.2 MHz is the clock rate
//   The timer divider is 12
//   The timer rate is 12 / 9.2 MHz = 1.304 us
//   The ticks are 1000 ms / (1.304 us) = 766.667
//   65536 - 766.667 = 64769.33
void delay_ms(int ms) {
    for (int i = 0; i < ms; i++) {
        delay_ticks(767);
    }
}
