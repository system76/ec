#include <board/keymap.h>

// Keymap layers (normal, Fn)
#define KM_LAY 2
// Keymap output pins
#define KM_OUT 16
// Keymap input pins
#define KM_IN 8

uint16_t __code KEYMAP[KM_IN][KM_OUT][KM_LAY] = {
    // TODO
    { { 0 } }
};

uint16_t keymap(int layer, int output, int input) {
    if (layer < KM_LAY && output < KM_OUT && input < KM_IN) {
        return KEYMAP[input][output][layer];
    } else {
        return 0;
    }
}
