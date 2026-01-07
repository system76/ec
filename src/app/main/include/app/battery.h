// SPDX-License-Identifier: GPL-3.0-only

#ifndef _APP_BATTERY_H
#define _APP_BATTERY_H

#include <common/macro.h>

#include <stdbool.h>
#include <stdint.h>

#ifndef BATTERY_ADDRESS
#define BATTERY_ADDRESS 0x0B
#endif

#ifndef CHARGER_ADDRESS
#define CHARGER_ADDRESS 0x09
#endif

#define BATTERY_FULLY_DISCHARGED BIT(4)
#define BATTERY_FULLY_CHARGED BIT(5)
#define BATTERY_DISCHARGING BIT(6)
#define BATTERY_INITIALIZED BIT(7)

struct battery_info {
    uint16_t temp;
    uint16_t voltage;
    uint16_t current;
    uint16_t charge;
    uint16_t remaining_capacity;
    uint16_t full_capacity;
    uint16_t status;
    uint16_t cycle_count;
    uint16_t design_capacity;
    uint16_t design_voltage;
};
extern struct battery_info battery_info;

extern uint16_t battery_charger_input_current;

uint8_t battery_get_start_threshold(void);
bool battery_set_start_threshold(uint8_t value);

uint8_t battery_get_end_threshold(void);
bool battery_set_end_threshold(uint8_t value);

bool battery_load_thresholds(void);
bool battery_save_thresholds(void);

int16_t battery_charger_configure(void);
void battery_event(void);
void battery_reset(void);

// Defined by charger/*.c
int16_t battery_charger_disable(void);
int16_t battery_charger_enable(void);
void battery_charger_event(void);
void battery_debug(void);

#endif // _APP_BATTERY_H
