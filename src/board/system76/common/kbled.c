// SPDX-License-Identifier: GPL-3.0-only

#include <board/kbled.h>
#include <board/options.h>
#include <common/macro.h>

enum KbledKind kbled_kind = KBLED_NONE;

// clang-format off
#ifdef KBLED_DAC
// XXX: DAC uses separate levels due to brightness being different.
static const uint8_t __code LEVELS[] = {
    0,
    128,
    144,
    168,
    192,
    255,
};
#else
static const uint8_t __code LEVELS[] = {
    48,
    72,
    96,
    144,
    192,
    255,
};
#endif

static const uint32_t __code COLORS[] = {
    0xFFFFFF,
    0x0000FF,
    0xFF0000,
    0xFF00FF,
    0x00FF00,
    0x00FFFF,
    0xFFFF00
};
// clang-format on

static bool enabled = false;
static uint8_t brightness = 0;

void kbled_restore(void) {
    kbled_set_brightness(LEVELS[options_get(OPT_KBLED_LEVEL_I)]);
    kbled_set_color(COLORS[options_get(OPT_KBLED_COLOR_I)]);
}

void kbled_enable(bool enable) {
    enabled = enable;

    if (enabled) {
        kbled_set(brightness);
    } else {
        kbled_set(0);
    }
}

void kbled_set_brightness(uint8_t value) {
    brightness = value;
    if (enabled) {
        kbled_set(brightness);
    }
}

void kbled_hotkey_color(void) {
    uint8_t color_i = options_get(OPT_KBLED_COLOR_I);
    if (color_i < (ARRAY_SIZE(COLORS) - 1)) {
        color_i += 1;
    } else {
        color_i = 0;
    }
    kbled_set_color(COLORS[color_i]);
    options_set(OPT_KBLED_COLOR_I, color_i);
}

void kbled_hotkey_down(void) {
    uint8_t level = options_get(OPT_KBLED_LEVEL_I);
    if (level > 0) {
        level -= 1;
    }
    kbled_set_brightness(LEVELS[level]);
    options_set(OPT_KBLED_LEVEL_I, level);
}

void kbled_hotkey_up(void) {
    uint8_t level = options_get(OPT_KBLED_LEVEL_I);
    if (level < (ARRAY_SIZE(LEVELS) - 1)) {
        level += 1;
    }
    kbled_set_brightness(LEVELS[level]);
    options_set(OPT_KBLED_LEVEL_I, level);
}

void kbled_hotkey_toggle(void) {
    uint8_t level = options_get(OPT_KBLED_LEVEL_I);
    if (kbled_get() == 0) {
        kbled_set_brightness(LEVELS[level]);
    } else {
        kbled_set_brightness(0);
    }
}

// Change the backlight level to the next value, cycling through "off".
void kbled_hotkey_step(void) {
    uint8_t level = options_get(OPT_KBLED_LEVEL_I);
    if (level < (ARRAY_SIZE(LEVELS) - 1)) {
        level += 1;
    } else {
        level = 0;
    }
    kbled_set_brightness(LEVELS[level]);
    options_set(OPT_KBLED_LEVEL_I, level);
}
