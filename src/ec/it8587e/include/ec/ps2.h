#ifndef _EC_PS2_H
#define _EC_PS2_H

#include <stdint.h>

__xdata volatile uint8_t __at(0x1700) PSCTL1;
__xdata volatile uint8_t __at(0x1701) PSCTL2;
__xdata volatile uint8_t __at(0x1702) PSCTL3;

__xdata volatile uint8_t __at(0x1704) PSINT1;
__xdata volatile uint8_t __at(0x1705) PSINT2;
__xdata volatile uint8_t __at(0x1706) PSINT3;

__xdata volatile uint8_t __at(0x1708) PSSTS1;
__xdata volatile uint8_t __at(0x1709) PSSTS2;
__xdata volatile uint8_t __at(0x170A) PSSTS3;

__xdata volatile uint8_t __at(0x170C) PSDAT1;
__xdata volatile uint8_t __at(0x170D) PSDAT2;
__xdata volatile uint8_t __at(0x170E) PSDAT3;

#endif // _EC_PS2_H
