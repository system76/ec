// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2019 System76, Inc.

#ifndef _GPIO_H
#define _GPIO_H

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

struct Gpio {
    volatile uint8_t *pin;
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    uint8_t value;
};

#define GPIO(BLOCK, NUMBER) { \
    .pin = &PIN ## BLOCK, \
    .ddr = &DDR ## BLOCK, \
    .port = &PORT ## BLOCK, \
    .value = (1U << (NUMBER)), \
}

bool gpio_get(const struct Gpio *const gpio);
void gpio_set(struct Gpio *const gpio, bool value);
bool gpio_get_dir(const struct Gpio *const gpio);
void gpio_set_dir(struct Gpio *const gpio, bool value);

#endif // _GPIO_H
