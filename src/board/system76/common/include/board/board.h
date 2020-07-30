#ifndef _BOARD_BOARD_H
#define _BOARD_BOARD_H

#include <stdbool.h>

void board_init(void);
void board_event(void);
void board_on_ac(bool ac);
void board_on_cpu_reset(void);

#endif // _BOARD_BOARD_H
