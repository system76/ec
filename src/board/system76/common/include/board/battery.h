// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_BATTERY_H
#define _BOARD_BATTERY_H

#include <stdbool.h>
#include <stdint.h>

#ifndef BATTERY_ADDRESS
    #define BATTERY_ADDRESS 0x0B
#endif

#ifndef CHARGER_ADDRESS
    #define CHARGER_ADDRESS 0x09
#endif

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

uint8_t battery_get_start_threshold(void);
bool battery_set_start_threshold(uint8_t value);

uint8_t battery_get_end_threshold(void);
bool battery_set_end_threshold(uint8_t value);

int battery_charger_configure(void);
void battery_event(void);
void battery_reset(void);

// Defined by charger/*.c
int battery_charger_disable(void);
int battery_charger_enable(void);
void battery_charger_event(void);
void battery_debug(void);

#endif // _BOARD_BATTERY_H
