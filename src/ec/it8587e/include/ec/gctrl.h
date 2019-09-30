#ifndef _EC_GCTRL_H
#define _EC_GCTRL_H

#include <stdint.h>

__xdata volatile uint8_t __at(0x2006) RSTS;
__xdata volatile uint8_t __at(0x200A) BADRSEL;
__xdata volatile uint8_t __at(0x200D) SPCTRL1;

#endif // _EC_GCTRL_H
