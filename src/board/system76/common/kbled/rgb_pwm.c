// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <board/kbled.h>
#include <board/options.h>
#include <ec/pwm.h>

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
    kbled_set_brightness(options_get(OPT_KBLED_BRIGHTNESS));
    if (gpio_get(&LID_SW_N))
        kbled_enable(true);
    kbled_set_color(
          (uint32_t)options_get(OPT_KBLED_COLOR_B)
        | (uint32_t)options_get(OPT_KBLED_COLOR_G) << 8
        | (uint32_t)options_get(OPT_KBLED_COLOR_R) << 16
        );
}

uint8_t kbled_get(void) {
    // Get PWM for power
    return DCR0;
}

uint8_t kbled_max(void) {
    return 255;
}

void kbled_set(uint8_t level) {
    // Set PWM for power
    DCR0 = level;
}

uint32_t kbled_get_color(void) {
    if (kbled_kind == KBLED_WHITE)
        return 0xFFFFFF;

    // Get PWM of blue component
    uint32_t color = (uint32_t)DCR7;

    // Get PWM of green component
    color |= ((uint32_t)DCR6) << 8;

    // Get PWM of red component
    color |= ((uint32_t)DCR5) << 16;

    return color;
}

void kbled_set_color(uint32_t color) {
    if (kbled_kind == KBLED_WHITE)
        color = 0xFFFFFF;

    // Set PWM for blue component
    DCR7 = (uint8_t)(color);

    // Set PWM for green component
    DCR6 = (uint8_t)(color >> 8);

    // Set PWM for red component
    DCR5 = (uint8_t)(color >> 16);
}
