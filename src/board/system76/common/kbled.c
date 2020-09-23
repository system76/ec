// SPDX-License-Identifier: GPL-3.0-only

#include <board/kbled.h>
#include <common/macro.h>

static int LEVEL_I = 1;
static const uint8_t __code LEVELS[] = {
    48,
    72,
    96,
    144,
    192,
    255
};

static int COLOR_I = 0;
static const uint32_t __code COLORS[] = {
    0xFFFFFF,
    0x0000FF,
    0xFF0000,
    0xFF00FF,
    0x00FF00,
    0x00FFFF,
    0xFFFF00
};

void kbled_hotkey_color(void) {
    if (COLOR_I < (ARRAY_SIZE(COLORS) - 1)) {
        COLOR_I += 1;
    } else {
        COLOR_I = 0;
    }
    kbled_set_color(COLORS[COLOR_I]);
}

void kbled_hotkey_down(void) {
    if (LEVEL_I > 0) {
        LEVEL_I -= 1;
    }
    kbled_set(LEVELS[LEVEL_I]);
}

void kbled_hotkey_up(void) {
    if (LEVEL_I < (ARRAY_SIZE(LEVELS) - 1)) {
        LEVEL_I += 1;
    }
    kbled_set(LEVELS[LEVEL_I]);
}

void kbled_hotkey_toggle(void) {
    if (kbled_get() == 0) {
        kbled_set(LEVELS[LEVEL_I]);
    } else {
        kbled_set(0);
    }
}
