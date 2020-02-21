#ifndef _BOARD_KBLED_H
#define _BOARD_KBLED_H

#include <stdint.h>

void kbled_init(void);
uint8_t kbled_get(void);
void kbled_set(uint8_t level);
void kbled_set_color(uint32_t color);

#endif // _BOARD_KBLED_H
