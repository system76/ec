// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2019 System76, Inc.

#include <ec/gpio.h>
#include <common/debug.h>

bool gpio_get(const struct Gpio *const gpio) {
    if (*(gpio->data) & gpio->value) {
        return true;
    } else {
        return false;
    }
}

void gpio_set(struct Gpio *const gpio, bool value) {
    if (value) {
        *(gpio->data) |= gpio->value;
    } else {
        *(gpio->data) &= ~(gpio->value);
    }
}

#ifdef GPIO_DEBUG
static void gpio_debug_bank(
    char *const bank,
    uint8_t data,
    uint8_t mirror,
    uint8_t pot,
    volatile uint8_t *const control
) {
    for (char i = 0; i < 8; i++) {
        DEBUG(
            "%s%d: data %d mirror %d pot %d control %02X\n",
            bank,
            i,
            (data >> i) & 1,
            (mirror >> i) & 1,
            (pot >> i) & 1,
            *(control + i)
        );
    }
}

void gpio_debug(void) {
#define bank(BANK) gpio_debug_bank(#BANK, GPDR##BANK, GPDMR##BANK, GPOT##BANK, &GPCR##BANK##0)
    bank(A);
    bank(B);
    bank(C);
    bank(D);
    bank(E);
    bank(F);
    bank(G);
    bank(H);
    bank(I);
    bank(J);
#define GPOTM 0
    bank(M);
#undef GPOTM
#undef bank
}
#endif // GPIO_DEBUG
