// SPDX-License-Identifier: GPL-3.0-only

#include <ec/wuc.h>
#include <common/macro.h>
#include <stddef.h>

struct WucGroup {
    volatile uint8_t *edge;
    volatile uint8_t *sense;
    volatile uint8_t *enable;
};

// Only groups 1, 3, and 4 have WUENR.
#define WUC_GROUP_EN(nr) { \
    .edge = &WUEMR ## nr, \
    .sense = &WUESR ## nr, \
    .enable = &WUENR ## nr, \
}

#define WUC_GROUP(nr) { \
    .edge = &WUEMR ## nr, \
    .sense = &WUESR ## nr, \
    .enable = NULL, \
}

static const struct WucGroup wuc[] = {
    // Group 0 unused
    WUC_GROUP_EN(1),
    WUC_GROUP(2),
    WUC_GROUP_EN(3),
    WUC_GROUP_EN(4),
    // Group 5 unused
    WUC_GROUP(6),
    WUC_GROUP(7),
    WUC_GROUP(8),
    WUC_GROUP(9),
    WUC_GROUP(10),
    WUC_GROUP(11),
    WUC_GROUP(12),
    WUC_GROUP(13),
    WUC_GROUP(14),
};

void wuc_enable(uint8_t nr, enum WucEdgeMode detect) {
    // XXX: SDCC doesn't optimize division with power-of-2.
    const uint8_t group = nr >> 3U;
    const uint8_t bit = nr % 8U;

    if (detect == EDGE_RISING) {
        *(wuc[group].edge) &= ~BIT(bit);
    } else if (detect == EDGE_FALLING || detect == EDGE_BOTH) {
        *(wuc[group].edge) |= BIT(bit);
    }

    *(wuc[group].sense) = BIT(bit);

    if (wuc[group].enable) {
        *(wuc[group].enable) |= BIT(bit);
    }
}

void wuc_disable(uint8_t nr) {
    // XXX: SDCC doesn't optimize division with power-of-2.
    const uint8_t group = nr >> 3U;
    const uint8_t bit = nr % 8U;

    if (wuc[group].enable) {
        *(wuc[group].enable) &= ~BIT(bit);
    }
}

void wuc_clear(uint8_t nr) {
    // XXX: SDCC doesn't optimize division with power-of-2.
    const uint8_t group = nr >> 3U;
    const uint8_t bit = nr % 8U;

    *(wuc[group].sense) = BIT(bit);
}
