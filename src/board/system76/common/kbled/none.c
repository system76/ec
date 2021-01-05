// SPDX-License-Identifier: GPL-3.0-only

#include <board/kbled.h>

void kbled_init(void) {}

void kbled_reset(void) {}

uint8_t kbled_get(void) { /*Always off*/ return 0; }

uint8_t kbled_max(void) { /*Always off*/ return 0; }

void kbled_set(uint8_t level) { /*Fix unused variable*/ level = level; }

uint32_t kbled_get_color(void) { /*Always black*/ return 0; }

void kbled_set_color(uint32_t color) { /*Fix unused variable*/ color = color; }
