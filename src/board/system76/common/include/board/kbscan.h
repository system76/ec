#ifndef _BOARD_KBSCAN_H
#define _BOARD_KBSCAN_H

#include <stdbool.h>

#include <ec/kbscan.h>

extern bool kbscan_enabled;

// ms between repeating key
extern uint16_t kbscan_repeat_period;
// ms between pressing key and repeating
extern uint16_t kbscan_repeat_delay;

void kbscan_init(void);
void kbscan_event(void);

#endif // _BOARD_KBSCAN_H
