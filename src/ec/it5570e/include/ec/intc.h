// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_INTC_H
#define _EC_INTC_H

#include <stdint.h>

#define IRQ_POST_CODE           155
#define IRQ_PLLFREQ_CHANGE      159

void interrupt_init(void);
void interrupt_enable(uint8_t irq);
void interrupt_clear(uint8_t irq);

void isr_post_code(void);

#endif // _EC_INTC_H
