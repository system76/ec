// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_BOARD_H
#define _BOARD_BOARD_H

#include <stdbool.h>

void board_init(void);
void board_event(void);
void board_on_ac(bool ac);

#endif // _BOARD_BOARD_H
