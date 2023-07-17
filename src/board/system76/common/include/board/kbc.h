// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_KBC_H
#define _BOARD_KBC_H

#include <stdbool.h>

#include <ec/kbc.h>

extern uint8_t kbc_leds;

void kbc_init(void);
void kbc_reset(void);
bool kbc_scancode(uint16_t key, bool pressed);
void kbc_event(struct Kbc *kbc);

#endif // _BOARD_KBC_H
