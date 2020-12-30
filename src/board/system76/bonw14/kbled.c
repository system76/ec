// SPDX-License-Identifier: GPL-3.0-only

#include <board/kbled.h>
#include <common/debug.h>
#include <ec/i2c.h>
#include <ec/pwm.h>
#include <ec/smbus.h>

void kbled_init(void) {
    i2c_reset(&I2C_DGPU, true);

    // Force SMBUS B design to 100kHZ
    SCLKTSB = 0b10;
}

void kbled_reset(void) {
    uint8_t value = 0xE4;
    int res = i2c_set(&I2C_DGPU, 0x2D, 0xA0, &value, 1);
    DEBUG("kbled_reset 0x2D: %d\n", res);

    //value = 0xC4;
    //res = i2c_set(&I2C_DGPU, 0x66, 0xA0, &value, 1);
    //DEBUG("kbled_reset 0x66: %d\n", res);

    // Set brightness and color
    uint8_t level = 0xFFFFFF;
    uint32_t color = 0x00;
}
