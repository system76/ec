#include <board/kbled.h>
#include <common/macro.h>
#include <ec/i2c.h>

#define kbled_i2c_get(A, D, L) i2c_get(&I2C_KBLED, 0x68, A, D, L)
#define kbled_i2c_set(A, D, L) i2c_set(&I2C_KBLED, 0x68, A, D, L)

static uint8_t __code levels[] = {
    0x00,
    0x80,
    0x90,
    0xA8,
    0xC0,
    0xFF
};

void kbled_init(void) {
    i2c_reset(&I2C_KBLED, true);
}

void kbled_reset(void) {
    kbled_set(0xFF);
    kbled_set_color(0xFFFFFF);
    
    // Enable output of channel 0 - 8
    uint8_t value = 0xFF;
    kbled_i2c_set(0x14, &value, 1);
    value = 0xFF;
    kbled_i2c_set(0x15, &value, 1);
    value = 0x03;
    kbled_i2c_set(0x16, &value, 1);

    // Exit sleep mode
    value = 0;
    kbled_i2c_set(0x00, &value, 1);
}

uint8_t kbled_get(void) {
    uint8_t level;
    uint8_t raw = 0;
    kbled_i2c_get(0x12, &raw, 1);
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
    kbled_i2c_set(0x12, &raw, 1);
}

void kbled_set_color(uint32_t color) {
    // Set channel 0 - 2 to blue component
    uint8_t b = (uint8_t)(color);
    kbled_i2c_set(0x02, &b, 1);
    kbled_i2c_set(0x03, &b, 1);
    kbled_i2c_set(0x04, &b, 1);

    // Set channel 3 - 5 to green component
    uint8_t g = (uint8_t)(color >> 8);
    kbled_i2c_set(0x05, &g, 1);
    kbled_i2c_set(0x06, &g, 1);
    kbled_i2c_set(0x07, &g, 1);

    // Set channel 6 - 8 to red component
    uint8_t r = (uint8_t)(color >> 16);
    kbled_i2c_set(0x08, &r, 1);
    kbled_i2c_set(0x09, &r, 1);
    kbled_i2c_set(0x0A, &r, 1);
}
