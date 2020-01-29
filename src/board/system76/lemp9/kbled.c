#include <board/dac.h>
#include <board/kbled.h>
#include <common/macro.h>

static uint8_t __code levels[] = {
    0x00,
    0x80,
    0x90,
    0xA8,
    0xC0,
    0xFF
};

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
