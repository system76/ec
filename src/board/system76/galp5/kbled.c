// SPDX-License-Identifier: GPL-3.0-only

#include <board/kbled.h>
#include <common/macro.h>
#include <ec/dac.h>

#define KBLED_DAC 2
#define KBLED_DACDAT DACDAT2

static uint8_t __code levels[] = {
    0x00,
    0x80,
    0x90,
    0xA8,
    0xC0,
    0xFF
};

void kbled_init(void) {
    // Enable DAC used for KBLIGHT_ADJ
    DACPDREG &= ~(1 << KBLED_DAC);
    kbled_reset();
}

void kbled_reset(void) {
    kbled_set(0);
}

uint8_t kbled_get(void) {
    uint8_t level;
    uint8_t raw = KBLED_DACDAT;
    for (level = 0; level < ARRAY_SIZE(levels); level++) {
        if (raw <= levels[level]) {
            return level;
        }
    }
    return 0;
}

void kbled_set(uint8_t level) {
    uint8_t raw = 0;
    if (level < ARRAY_SIZE(levels)) {
        raw = levels[level];
    }
    KBLED_DACDAT = raw;
}

void kbled_set_color(uint32_t color) { /*Fix unused variable*/ color = color; }
