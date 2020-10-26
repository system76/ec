// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_INTC_H
#define _EC_INTC_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x1110) IVECT;
volatile uint8_t __xdata __at(0x1150) ISR19;
volatile uint8_t __xdata __at(0x1151) IER19;

#endif // _EC_INTC_H
