#include <board/dac.h>
#include <board/kbled.h>
#include <common/macro.h>

static uint8_t __code levels[] = {
    0,
    48,
    72,
    96,
    192,
    255
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
    if (level < ARRAY_SIZE(levels)) {
        DACDAT2 = levels[level];
    } else {
        DACDAT2 = 0;
    }
}
