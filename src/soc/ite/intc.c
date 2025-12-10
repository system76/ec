// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2023 System76, Inc.

#include <soc/intc.h>
#include <common/macro.h>

struct IrqGroup {
    volatile uint8_t *status;
    volatile uint8_t *enable;
    volatile uint8_t *level;
    volatile uint8_t *polarity;
};

#define IRQ_GROUP(nr) { \
    .status = &ISR ## nr, \
    .enable = &IER ## nr, \
    .level = &IELMR ## nr, \
    .polarity = &IPOLR ## nr, \
}

static const struct IrqGroup irqs[] = {
    IRQ_GROUP(0),
    IRQ_GROUP(1),
    IRQ_GROUP(2),
    IRQ_GROUP(3),
    IRQ_GROUP(4),
    IRQ_GROUP(5),
    IRQ_GROUP(6),
    IRQ_GROUP(7),
    IRQ_GROUP(8),
    IRQ_GROUP(9),
    IRQ_GROUP(10),
    IRQ_GROUP(11),
    IRQ_GROUP(12),
    IRQ_GROUP(13),
    IRQ_GROUP(14),
    IRQ_GROUP(15),
    IRQ_GROUP(16),
    IRQ_GROUP(17),
    IRQ_GROUP(18),
#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
    IRQ_GROUP(19),
    IRQ_GROUP(20),
    IRQ_GROUP(21),
#endif
};

void intc_enable(uint8_t nr) {
    // XXX: SDCC doesn't optimize division with power-of-2.
    const uint8_t group = nr >> 3U;
    const uint8_t bit = nr % 8U;

    *(irqs[group].status) = BIT(bit);
    *(irqs[group].enable) |= BIT(bit);
}

void intc_disable(uint8_t nr) {
    // XXX: SDCC doesn't optimize division with power-of-2.
    const uint8_t group = nr >> 3U;
    const uint8_t bit = nr % 8U;

    *(irqs[group].enable) &= ~BIT(bit);
}

void intc_clear(uint8_t nr) {
    // XXX: SDCC doesn't optimize division with power-of-2.
    const uint8_t group = nr >> 3U;
    const uint8_t bit = nr % 8U;

    *(irqs[group].status) = BIT(bit);
}
