// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_OPTIONS_H
#define _BOARD_OPTIONS_H

#include <stdbool.h>
#include <stdint.h>

// Initialize the options
void options_init(void);
// Set the options to the default options
void options_load_default(void);
// Erase options in flash
bool options_erase_config(void);
// Load options from flash
bool options_load_config(void);
// Save options to flash
bool options_save_config(void);
// Get an option
uint8_t options_get(uint16_t index);
// Set an option
bool options_set(uint16_t index, uint8_t value);

enum {
    OPT_POWER_ON_AC = 0,
    NUM_OPTIONS
};

#endif // _BOARD_OPTIONS_H
