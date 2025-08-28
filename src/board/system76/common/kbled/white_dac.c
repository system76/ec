// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

#include <board/kbled.h>
#include <common/macro.h>
#include <ec/dac.h>

#if !defined(KBLED_DAC)
#error "KBLED_DAC must be defined"
#endif

#define KBLED_DACDAT xconcat(DACDAT, KBLED_DAC)

void kbled_init(void) {
    kbled_kind = KBLED_WHITE;

    // Enable DAC used for KBLIGHT_ADJ
    DACPDREG &= ~BIT(KBLED_DAC);
    kbled_reset();
}

void kbled_reset(void) {
    kbled_set(0);
}

uint8_t kbled_get(void) {
    return KBLED_DACDAT;
}

uint8_t kbled_max(void) {
    return 0xFF;
}

void kbled_set(uint8_t level) {
    KBLED_DACDAT = level;
}

uint32_t kbled_get_color(void) {
    // Always white
    return 0xFFFFFF;
}

void kbled_set_color(uint32_t color) {
    // Fix unused variable
    color = color;
}
