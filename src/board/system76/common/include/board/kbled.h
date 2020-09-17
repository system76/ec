// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_KBLED_H
#define _BOARD_KBLED_H

#include <stdint.h>

void kbled_init(void);
void kbled_reset(void);
uint8_t kbled_get(void);
void kbled_set(uint8_t level);
void kbled_set_color(uint32_t color);

#endif // _BOARD_KBLED_H
