#include <board/kbled.h>
#include <common/macro.h>
#include <ec/dac.h>

static uint8_t __code levels[] = {
    0x00,
    0x80,
    0x90,
    0xA8,
    0xC0,
    0xFF
};

void kbled_init(void) {
    // Enable DAC2, used for KBLIGHT_ADJ
    DACPDREG &= ~(1 << 2);
    // Set DAC2 to 0V
    DACDAT2 = 0;
}

uint8_t kbled_get(void) {
    uint8_t level;
    uint8_t raw = DACDAT2;
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
    DACDAT2 = raw;
}
