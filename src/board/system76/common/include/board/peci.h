#ifndef _BOARD_PECI_H
#define _BOARD_PECI_H

#include <ec/peci.h>

extern int16_t peci_offset;
extern int16_t peci_temp;
extern uint8_t peci_duty;
extern uint8_t peci_tcontrol;
extern uint8_t peci_tjmax;

void peci_init(void);
void peci_event(void);

#endif // _BOARD_PECI_H
