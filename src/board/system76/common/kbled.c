// SPDX-License-Identifier: GPL-3.0-only

#include <board/kbled.h>
#include <board/options.h>
#include <common/macro.h>

enum KbledKind kbled_kind = KBLED_NONE;

// clang-format off
static uint8_t LEVEL_I = 1;
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

static uint8_t COLOR_I = 0;
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
    options_set(OPT_KBLED_BRIGHTNESS, brightness);
}

void kbled_hotkey_color(void) {
    if (COLOR_I < (ARRAY_SIZE(COLORS) - 1)) {
        COLOR_I += 1;
    } else {
        COLOR_I = 0;
    }
    kbled_set_color(COLORS[COLOR_I]);
    options_set(OPT_KBLED_COLOR_B, (uint8_t)COLORS[COLOR_I]);
    options_set(OPT_KBLED_COLOR_G, (uint8_t)(COLORS[COLOR_I] >> 8));
    options_set(OPT_KBLED_COLOR_R, (uint8_t)(COLORS[COLOR_I] >> 16));
}

void kbled_hotkey_down(void) {
    if (LEVEL_I > 0) {
        LEVEL_I -= 1;
    }
    kbled_set_brightness(LEVELS[LEVEL_I]);
}

void kbled_hotkey_up(void) {
    if (LEVEL_I < (ARRAY_SIZE(LEVELS) - 1)) {
        LEVEL_I += 1;
    }
    kbled_set_brightness(LEVELS[LEVEL_I]);
}

void kbled_hotkey_toggle(void) {
    if (kbled_get() == 0) {
        kbled_set_brightness(LEVELS[LEVEL_I]);
    } else {
        kbled_set_brightness(0);
    }
}

// Change the backlight level to the next value, cycling through "off".
void kbled_hotkey_step(void) {
    if (LEVEL_I < (ARRAY_SIZE(LEVELS) - 1)) {
        LEVEL_I += 1;
    } else {
        LEVEL_I = 0;
    }
    kbled_set_brightness(LEVELS[LEVEL_I]);
}
