// SPDX-License-Identifier: GPL-3.0-only

#ifndef _ARCH_GPIO_H
#define _ARCH_GPIO_H

#include <common/macro.h>

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

struct Gpio {
    volatile uint8_t * pin;
    volatile uint8_t * ddr;
    volatile uint8_t * port;
    uint8_t value;
};

#define GPIO(BLOCK, NUMBER) { \
    .pin = &PIN ## BLOCK, \
    .ddr = &DDR ## BLOCK, \
    .port = &PORT ## BLOCK, \
    .value = BIT(NUMBER), \
}

bool gpio_get(struct Gpio * gpio);
void gpio_set(struct Gpio * gpio, bool value);
bool gpio_get_dir(struct Gpio * gpio);
void gpio_set_dir(struct Gpio * gpio, bool value);

#endif // _ARCH_GPIO_H
