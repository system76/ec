#ifndef _BOARD_KEYMAP_H
#define _BOARD_KEYMAP_H

#include <common/keymap.h>

// Keymap output pins (16 for galago, 18 for darter)
#define KM_OUT 18
// Keymap input pins
#define KM_IN 8
// Keymap layers (normal, Fn)
#define KM_LAY 2

// Keymap
extern uint16_t __code KEYMAP[KM_LAY][KM_OUT][KM_IN];

// Get a keycode from the keymap
uint16_t keymap(int output, int input, int layer);

#endif // _BOARD_KEYMAP_H
