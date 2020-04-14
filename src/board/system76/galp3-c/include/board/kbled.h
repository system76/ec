#ifndef _BOARD_KBLED_H
#define _BOARD_KBLED_H

#include <stdint.h>

void kbled_init(void);
void kbled_reset(void);
uint8_t kbled_get(void);
void kbled_set(uint8_t level);

#endif // _BOARD_KBLED_H
