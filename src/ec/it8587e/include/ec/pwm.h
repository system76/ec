#ifndef _EC_PWM_H
#define _EC_PWM_H

#include <stdint.h>

// Duty cycle register 0
__xdata volatile uint8_t __at(0x1802) DCR0;
// Duty cycle register 1
__xdata volatile uint8_t __at(0x1803) DCR1;
// Duty cycle register 2
__xdata volatile uint8_t __at(0x1804) DCR2;
// Duty cycle register 3
__xdata volatile uint8_t __at(0x1805) DCR3;
// Duty cycle register 4
__xdata volatile uint8_t __at(0x1806) DCR4;
// Duty cycle register 5
__xdata volatile uint8_t __at(0x1807) DCR5;
// Duty cycle register 6
__xdata volatile uint8_t __at(0x1808) DCR6;
// Duty cycle register 7
__xdata volatile uint8_t __at(0x1809) DCR7;

// Fan one tachometer least significant byte reading register
__xdata volatile uint8_t __at(0x181E) F1TLRR;
// Fan one tachometer most significant byte reading register
__xdata volatile uint8_t __at(0x181F) F1TMRR;
// Fan two tachometer least significant byte reading register
__xdata volatile uint8_t __at(0x1820) F2TLRR;
// Fan two tachometer most significant byte reading register
__xdata volatile uint8_t __at(0x1821) F2TMRR;
// Tachometer switch control register
__xdata volatile uint8_t __at(0x1848) TSWCTLR;

#endif // _EC_PWM_H
