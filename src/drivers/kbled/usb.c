// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

// USB HID interface for KBLED.

#include "kbled.h"
#include <common/debug.h>
#include <soc/i2c.h>
#include <soc/smbus.h>

void kbled_init(void) {
    kbled_kind = KBLED_RGB;

    i2c_reset(&KBLED_I2C, true);

    // Force SMBUS B design to 100kHZ
    SCLKTSB = 0b10;
}

void kbled_reset(void) {
    uint8_t value = 0xE4;
    int16_t res = i2c_set(&KBLED_I2C, 0x2D, 0xA0, &value, 1);
    DEBUG("kbled_reset 0x2D: %d\n", res);

    //value = 0xC4;
    //res = i2c_set(&KBLED_I2C, 0x66, 0xA0, &value, 1);
    //DEBUG("kbled_reset 0x66: %d\n", res);

    // Set brightness and color
    kbled_set_color(0xFFFFFF);
    kbled_set(0x00);
}

// Keep the following functions for compatibility - they are set via USB HID
uint8_t kbled_get(void) {
    // Always off
    return 0;
}

uint8_t kbled_max(void) {
    // Always off
    return 0;
}

void kbled_set(uint8_t level) {
    // Fix unused variable
    level = level;
}

uint32_t kbled_get_color(void) {
    // Always black
    return 0;
}

void kbled_set_color(uint32_t color) {
    // Fix unused variable
    color = color;
}
