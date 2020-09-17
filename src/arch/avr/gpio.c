// SPDX-License-Identifier: GPL-3.0-only

#include <arch/gpio.h>

bool gpio_get_dir(struct Gpio * gpio) {
    if (*gpio->ddr & gpio->value) {
        return true;
    } else {
        return false;
    }
}

void gpio_set_dir(struct Gpio * gpio, bool value) {
    if (value) {
        *gpio->ddr |= gpio->value;
    } else {
        *gpio->ddr &= ~gpio->value;
    }
}

bool gpio_get(struct Gpio * gpio) {
    if (*gpio->pin & gpio->value) {
        return true;
    } else {
        return false;
    }
}

void gpio_set(struct Gpio * gpio, bool value) {
    if (value) {
        *gpio->port |= gpio->value;
    } else {
        *gpio->port &= ~gpio->value;
    }
}
