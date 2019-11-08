#ifndef _BOARD_KBC_H
#define _BOARD_KBC_H

#include <ec/kbc.h>

void kbc_init(void);
void kbc_event(struct Kbc * kbc);

#endif // _BOARD_KBC_H
