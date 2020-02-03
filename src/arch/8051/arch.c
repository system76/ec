#include <8051.h>

#include <arch/time.h>

void external_0(void) __interrupt(0) {}
// timer_0 is in time.c
void external_1(void) __interrupt(2) {}
void timer_1(void) __interrupt(3) {}
void serial(void) __interrupt(4) {}
void timer_2(void) __interrupt(5) {}

void arch_init(void) {
    // Disable interrupts
    EA = 0;

    time_init();

    // Enable interrupts
    EA = 1;
}
