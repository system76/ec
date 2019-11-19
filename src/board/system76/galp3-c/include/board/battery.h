#ifndef _BOARD_BATTERY_H
#define _BOARD_BATTERY_H

#include <stdint.h>

extern uint16_t battery_temp;
extern uint16_t battery_voltage;
extern uint16_t battery_current;
extern uint16_t battery_charge;

int battery_charger_disable(void);
int battery_charger_enable(void);
void battery_event(void);
void battery_debug(void);

#endif // _BOARD_BATTERY_H
