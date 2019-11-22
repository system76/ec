#ifndef _ARCH_DELAY_H
#define _ARCH_DELAY_H

#include <stdint.h>

void delay_ticks(uint16_t ticks);

// 1 us * 9.2 MHz / 12 is 69/90
// Warning: this will round to the nearest tick
#define delay_us(X) \
    delay_ticks((uint16_t)((((uint32_t)(X)) * 69UL + 89UL) / 90UL));

// 1 ns * 9.2 MHz / 12 is 69/90000
// Warning: this will round to the nearest tick
#define delay_ns(X) \
    delay_ticks((uint16_t)((((uint32_t)(X)) * 69UL + 89999UL) / 90000UL));
    
void delay_ms(int ms);

#endif // _ARCH_DELAY_H
