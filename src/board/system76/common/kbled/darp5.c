// SPDX-License-Identifier: GPL-3.0-only

#include <arch/delay.h>
#include <board/gpio.h>
#include <board/kbled.h>
#include <common/macro.h>
#include <ec/i2c.h>

#define kbled_i2c_get(A, D, L) i2c_get(&I2C_KBLED, 0x68, A, D, L)
#define kbled_i2c_set(A, D, L) i2c_set(&I2C_KBLED, 0x68, A, D, L)

void kbled_init(void) {
    if (!gpio_get(&RGBKB_DET_N)) {
        kbled_kind = KBLED_RGB;
    } else {
        kbled_kind = KBLED_WHITE;
    }

    i2c_reset(&I2C_KBLED, true);
}

void kbled_reset(void) {
    // Switch from sleep mode to normal mode
    uint8_t value = 0x01;
    kbled_i2c_set(0x00, &value, 1);

    // Wait for normal mode
    delay_ms(1);

    // Set brightness and color
    kbled_set_color(0xFFFFFF);
    kbled_set(0x00);

    // Enable output of channel 0 - 8
    value = 0xFF;
    kbled_i2c_set(0x14, &value, 1);
    value = 0xFF;
    kbled_i2c_set(0x15, &value, 1);
    value = 0x03;
    kbled_i2c_set(0x16, &value, 1);
}

uint8_t kbled_get(void) {
    uint8_t level;
    kbled_i2c_get(0x12, &level, 1);
    return level;
}

uint8_t kbled_max(void) {
    return 255;
}

void kbled_set(uint8_t level) {
    kbled_i2c_set(0x12, &level, 1);
}

uint32_t kbled_get_color(void) {
    if (kbled_kind == KBLED_WHITE)
        return 0xFFFFFF;

    // Get blue component from channel 0
    uint8_t value;
    kbled_i2c_get(0x02, &value, 1);
    uint32_t color = (uint32_t)value;

    // Get green component from channel 3
    kbled_i2c_get(0x05, &value, 1);
    color |= ((uint32_t)value) << 8;

    // Get red component from channel 6
    kbled_i2c_get(0x08, &value, 1);
    color |= ((uint32_t)value) << 16;

    return color;
}

void kbled_set_color(uint32_t color) {
    if (kbled_kind == KBLED_WHITE)
        color = 0xFFFFFF;

    // Set channel 0 - 2 to blue component
    uint8_t value = (uint8_t)(color);
    kbled_i2c_set(0x02, &value, 1);
    kbled_i2c_set(0x03, &value, 1);
    kbled_i2c_set(0x04, &value, 1);

    // Set channel 3 - 5 to green component
    value = (uint8_t)(color >> 8);
    kbled_i2c_set(0x05, &value, 1);
    kbled_i2c_set(0x06, &value, 1);
    kbled_i2c_set(0x07, &value, 1);

    // Set channel 6 - 8 to red component
    value = (uint8_t)(color >> 16);
    kbled_i2c_set(0x08, &value, 1);
    kbled_i2c_set(0x09, &value, 1);
    kbled_i2c_set(0x0A, &value, 1);
}
