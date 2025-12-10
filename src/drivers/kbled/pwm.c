// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

// PWM interface for KBLED.

#include "kbled.h"
#include <board/gpio.h>
#include <soc/pwm.h>

#define BRIGHTNESS_PWM DCR0
#define RED_PWM DCR5
#define GREEN_PWM DCR6
#define BLUE_PWM DCR7

void kbled_init(void) {
    if (!gpio_get(&RGBKB_DET_N)) {
        kbled_kind = KBLED_RGB;
    } else {
        kbled_kind = KBLED_WHITE;
    }

    //TODO: enable PWMs
    kbled_reset();
}

void kbled_reset(void) {
    // Set brightness and color
    kbled_set_color(0xFFFFFF);
    kbled_set(0x00);
}

uint8_t kbled_get(void) {
    // Get PWM for power
    return BRIGHTNESS_PWM;
}

uint8_t kbled_max(void) {
    return CTR0;
}

void kbled_set(uint8_t level) {
    // Set PWM for power
    BRIGHTNESS_PWM = level;
}

uint32_t kbled_get_color(void) {
    if (kbled_kind == KBLED_WHITE)
        return 0xFFFFFF;

    // Get PWM of blue component
    uint32_t color = (uint32_t)BLUE_PWM;

    // Get PWM of green component
    color |= ((uint32_t)GREEN_PWM) << 8;

    // Get PWM of red component
    color |= ((uint32_t)RED_PWM) << 16;

    return color;
}

void kbled_set_color(uint32_t color) {
    if (kbled_kind == KBLED_WHITE)
        color = 0xFFFFFF;

    // Set PWM for blue component
    BLUE_PWM = (uint8_t)(color);

    // Set PWM for green component
    GREEN_PWM = (uint8_t)(color >> 8);

    // Set PWM for red component
    RED_PWM = (uint8_t)(color >> 16);
}
