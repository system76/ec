#include <board/keymap.h>
#include <common/debug.h>

uint16_t keymap(int output, int input, int layer) {
    if (output < KM_OUT && input < KM_IN && layer < KM_LAY) {
        return KEYMAP[output][input][layer];
    } else {
        return 0;
    }
}

