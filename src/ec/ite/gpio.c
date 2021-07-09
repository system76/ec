// SPDX-License-Identifier: GPL-3.0-only

#include <ec/gpio.h>

bool gpio_get(struct Gpio * gpio) {
    if (*(gpio->data) & gpio->value) {
        return true;
    } else {
        return false;
    }
}

void gpio_set(struct Gpio * gpio, bool value) {
    if (value) {
        *(gpio->data) |= gpio->value;
    } else {
        *(gpio->data) &= ~(gpio->value);
    }
}
