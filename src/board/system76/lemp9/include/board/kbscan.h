#ifndef _BOARD_KBSCAN_H
#define _BOARD_KBSCAN_H

#include <stdbool.h>

#include <ec/kbscan.h>

extern bool kbscan_enabled;

void kbscan_init(void);
void kbscan_event(void);

#endif // _BOARD_KBSCAN_H
