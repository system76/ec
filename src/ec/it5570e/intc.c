#include <common/macro.h>
#include <ec/intc.h>

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

static const struct IrqGroup Irqs[] = {
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
    IRQ_GROUP(19),
    IRQ_GROUP(20),
    IRQ_GROUP(21),
};

void interrupt_enable(unsigned int nr) {
    unsigned int group = nr / 8;
    unsigned int bit = nr % 8;

    *(Irqs[group].status) = BIT(bit);
    *(Irqs[group].enable) |= BIT(bit);
}

void interrupt_disable(unsigned int nr) {
    unsigned int group = nr / 8;
    unsigned int bit = nr % 8;

    *(Irqs[group].enable) &= ~BIT(bit);
}

void interrupt_clear(unsigned int nr) {
    unsigned int group = nr / 8;
    unsigned int bit = nr % 8;

    *(Irqs[group].status) = BIT(bit);
}
