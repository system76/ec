// SPDX-License-Identifier: GPL-3.0-only

#include <ec/intc.h>
#include <common/debug.h>
#include <common/macro.h>
#include <stddef.h>

#define INTC_BASE 0x1100

static volatile uint8_t __xdata __at(INTC_BASE + 0x50) ISR19;
static volatile uint8_t __xdata __at(INTC_BASE + 0x51) IER19;

static volatile uint8_t __xdata __at(INTC_BASE + 0x10) IVCT;

static inline uint8_t interrupt_get_irq(void) {
    return IVCT - 0x10;
}

static inline void isr_noop(void) {}

static const struct {
    volatile __xdata uint8_t *status;
    volatile __xdata uint8_t *enable;
    void (*handler)(void);
} irqs[] = {
#if LEVEL >= DEBUG
    [IRQ_POST_CODE] = { &ISR19, &IER19, isr_post_code },
#endif
    [IRQ_PLLFREQ_CHANGE] = { &ISR19, &IER19, isr_noop },
};

void interrupt_enable(uint8_t irq) {
    uint8_t bit = irq & 0b111;

    *(irqs[irq].status) = BIT(bit);
    *(irqs[irq].enable) |= BIT(bit);
}

void interrupt_clear(uint8_t irq) {
    uint8_t bit = irq & 0b111;

    *(irqs[irq].status) = BIT(bit);
}

void interrupt_init(void) {
    for (uint8_t i = 0; i < ARRAY_SIZE(irqs); i++) {
        interrupt_enable(i);
    }
}

void external_1(void) __interrupt(2) {
    uint8_t irq = interrupt_get_irq();

    irqs[irq].handler();
    interrupt_clear(irq);
}
