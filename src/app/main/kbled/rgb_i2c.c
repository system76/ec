// SPDX-License-Identifier: GPL-3.0-only

#include <app/kbled.h>
#include <arch/delay.h>
#include <board/gpio.h>
#include <common/macro.h>
#include <ec/i2c.h>

#define kbled_i2c_get(A, D, L) i2c_get(&KBLED_I2C, 0x68, A, D, L)
#define kbled_i2c_set(A, D, L) i2c_set(&KBLED_I2C, 0x68, A, D, L)

#if CONFIG_KBLED_I2C_CH_BGR_PACKED
// Channels are in BBB|GGG|RRR format
#define BLUE_CH0    0x02
#define BLUE_CH1    0x03
#define BLUE_CH2    0x04
#define GREEN_CH0   0x05
#define GREEN_CH1   0x06
#define GREEN_CH2   0x07
#define RED_CH0     0x08
#define RED_CH1     0x09
#define RED_CH2     0x0A

#elif CONFIG_KBLED_I2C_CH_BGR_MIXED
// Channels are in BGR|BGR|BGR format
#define BLUE_CH0    0x02
#define BLUE_CH1    0x05
#define BLUE_CH2    0x08
#define GREEN_CH0   0x03
#define GREEN_CH1   0x06
#define GREEN_CH2   0x09
#define RED_CH0     0x04
#define RED_CH1     0x07
#define RED_CH2     0x0A

#else
#error Invalid I2C color channel layout
#endif

void kbled_init(void) {
    if (!gpio_get(&RGBKB_DET_N)) {
        kbled_kind = KBLED_RGB;
    } else {
        kbled_kind = KBLED_WHITE;
    }

    i2c_reset(&KBLED_I2C, true);
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

    // Get blue component
    uint8_t value;
    kbled_i2c_get(BLUE_CH0, &value, 1);
    uint32_t color = (uint32_t)value;

    // Get green component
    kbled_i2c_get(GREEN_CH0, &value, 1);
    color |= ((uint32_t)value) << 8;

    // Get red component
    kbled_i2c_get(RED_CH0, &value, 1);
    color |= ((uint32_t)value) << 16;

    return color;
}

void kbled_set_color(uint32_t color) {
    if (kbled_kind == KBLED_WHITE)
        color = 0xFFFFFF;

    // Set blue component
    uint8_t value = (uint8_t)(color);
    kbled_i2c_set(BLUE_CH0, &value, 1);
    kbled_i2c_set(BLUE_CH1, &value, 1);
    kbled_i2c_set(BLUE_CH2, &value, 1);

    // Set green component
    value = (uint8_t)(color >> 8);
    kbled_i2c_set(GREEN_CH0, &value, 1);
    kbled_i2c_set(GREEN_CH1, &value, 1);
    kbled_i2c_set(GREEN_CH2, &value, 1);

    // Set red component
    value = (uint8_t)(color >> 16);
    kbled_i2c_set(RED_CH0, &value, 1);
    kbled_i2c_set(RED_CH1, &value, 1);
    kbled_i2c_set(RED_CH2, &value, 1);
}
