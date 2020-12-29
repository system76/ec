// SPDX-License-Identifier: GPL-3.0-only

#include <board/battery.h>
#include <board/smbus.h>
#include <common/debug.h>

// Default values to disable battery charging thresholds
#define BATTERY_START_DEFAULT   0
#define BATTERY_END_DEFAULT     100

// Represents a battery percentage level, below which charging will begin.
// Valid values are [0, 100]
// A value of 0 turns off the start threshold control.
static uint8_t battery_start_threshold = BATTERY_START_THRESHOLD;

// Represents a battery percentage level, above which charging will stop.
// Valid values are [0, 100]
// A value of 100 turns off the stop threshold control.
static uint8_t battery_end_threshold = BATTERY_END_THRESHOLD;

uint8_t battery_get_start_threshold(void) {
    if (battery_start_threshold > 100)
        return BATTERY_START_DEFAULT;
    return battery_start_threshold;
}

bool battery_set_start_threshold(uint8_t value) {
    if (value > 100 || value >= battery_end_threshold)
        return false;

    battery_start_threshold = value;
    return true;
}

uint8_t battery_get_end_threshold(void) {
    if (battery_end_threshold > 100)
        return BATTERY_END_DEFAULT;
    return battery_end_threshold;
}

bool battery_set_end_threshold(uint8_t value) {
    if (value > 100 || value <= battery_start_threshold)
        return false;

    battery_end_threshold = value;
    return true;
}

/**
 * Configure the charger based on charging threshold values.
 */
int battery_charger_configure(void) {
    static bool should_charge = true;

    if (battery_get_end_threshold() == BATTERY_END_DEFAULT) {
        // Stop threshold not configured: Always charge on AC.
        should_charge = true;
    }
    else if (battery_charge >= battery_get_end_threshold()) {
        // Stop threshold configured: Stop charging at threshold.
        should_charge = false;
    }
    else if (battery_get_start_threshold() == BATTERY_START_DEFAULT) {
        // Start threshold not configured: Always charge up to stop threshold.
        should_charge = true;
    }
    else if (battery_charge <= battery_get_start_threshold()) {
        // Start threshold configured: Start charging at threshold.
        should_charge = true;
    }

    if (should_charge)
        return battery_charger_enable();
    return battery_charger_disable();
}

uint16_t battery_temp = 0;
uint16_t battery_voltage = 0;
uint16_t battery_current = 0;
uint16_t battery_charge = 0;
uint16_t battery_remaining_capacity = 0;
uint16_t battery_full_capacity = 0;
uint16_t battery_status = 0;
uint16_t battery_design_capacity = 0;
uint16_t battery_design_voltage = 0;

void battery_event(void) {
    int res = 0;

    #define command(N, V) { \
        res = smbus_read(BATTERY_ADDRESS, V, &N); \
        if (res < 0) { \
            N = 0; \
        } \
    }

    command(battery_temp, 0x08);
    command(battery_voltage, 0x09);
    command(battery_current, 0x0A);
    command(battery_charge, 0x0D);
    command(battery_remaining_capacity, 0x0F);
    command(battery_full_capacity, 0x10);
    command(battery_status, 0x16);
    command(battery_design_capacity, 0x18);
    command(battery_design_voltage, 0x19);

    #undef command

    DEBUG("BAT %d mV %d mA\n", battery_voltage, battery_current);

    battery_charger_event();
}

void battery_reset(void) {
    battery_start_threshold = BATTERY_START_THRESHOLD;
    battery_end_threshold = BATTERY_END_THRESHOLD;
}
