// SPDX-License-Identifier: GPL-3.0-only

#include <app/kbled.h>
#include <board/gpio.h>
#include <ec/pwm.h>

#define KBLED_BRI DCR0
#define KBLED_RED DCR5
#define KBLED_GREEN DCR6
#define KBLED_BLUE DCR7

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
    return KBLED_BRI;
}

uint8_t kbled_max(void) {
    return 255;
}

void kbled_set(uint8_t level) {
    KBLED_BRI = level;
}

uint32_t kbled_get_color(void) {
    if (kbled_kind == KBLED_WHITE)
        return 0xFFFFFF;

    uint32_t color = (uint32_t)KBLED_BLUE;
    color |= ((uint32_t)KBLED_GREEN) << 8;
    color |= ((uint32_t)KBLED_RED) << 16;

    return color;
}

void kbled_set_color(uint32_t color) {
    if (kbled_kind == KBLED_WHITE)
        color = 0xFFFFFF;

    KBLED_BLUE = (uint8_t)(color);
    KBLED_GREEN = (uint8_t)(color >> 8);
    KBLED_RED = (uint8_t)(color >> 16);
}
