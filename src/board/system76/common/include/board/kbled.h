// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_KBLED_H
#define _BOARD_KBLED_H

#include <stdint.h>

// Must be specified by board
void kbled_init(void);
void kbled_reset(void);
uint8_t kbled_get(void);
uint8_t kbled_max(void);
void kbled_set(uint8_t level);
uint32_t kbled_get_color(void);
void kbled_set_color(uint32_t color);

// Provided by common code
void kbled_hotkey_color(void);
void kbled_hotkey_down(void);
void kbled_hotkey_up(void);
void kbled_hotkey_toggle(void);

#endif // _BOARD_KBLED_H
