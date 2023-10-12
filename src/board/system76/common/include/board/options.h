// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_OPTIONS_H
#define _BOARD_OPTIONS_H

#include <stdbool.h>
#include <stdint.h>

// Initialize the options
void options_init(void);
// Save options to flash
bool options_save_config(void);
// Get an option
uint8_t options_get(uint16_t index);
// Set an option
bool options_set(uint16_t index, uint8_t value);

enum {
    OPT_POWER_ON_AC = 0,
    OPT_KBLED_BRIGHTNESS,
    OPT_KBLED_COLOR_R,
    OPT_KBLED_COLOR_G,
    OPT_KBLED_COLOR_B,
    OPT_BAT_THRESHOLD_START,
    OPT_BAT_THRESHOLD_STOP,
    NUM_OPTIONS
};

#endif // _BOARD_OPTIONS_H
