#ifndef _BOARD_BATTERY_H
#define _BOARD_BATTERY_H

#include <stdint.h>

#define BATTERY_INITIALIZED (1U << 7)

extern uint16_t battery_temp;
extern uint16_t battery_voltage;
extern uint16_t battery_current;
extern uint16_t battery_charge;
extern uint16_t battery_remaining_capacity;
extern uint16_t battery_full_capacity;
extern uint16_t battery_status;
extern uint16_t battery_design_capacity;
extern uint16_t battery_design_voltage;

int battery_charger_disable(void);
int battery_charger_enable(void);
void battery_event(void);
void battery_debug(void);

#endif // _BOARD_BATTERY_H
