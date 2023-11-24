// SPDX-License-Identifier: GPL-3.0-only
//
// O2 Micro OZ26786
// No public datasheet

#include <board/battery.h>
#include <board/smbus.h>
#include <common/debug.h>
#include <common/macro.h>

// clang-format off

// Registers
#define REG_CHARGE_CURRENT 0x14
#define REG_CHARGE_VOLTAGE 0x15
#define REG_CHARGE_OPTION_1 0x3D
    #define CHARGE_OPTION_1_HPB_EN BIT(10)
    #define CHARGE_OPTION_1_600KHZ BIT(11)
#define REG_CHARGE_OPTION_2 0x3B
    #define CHARGE_OPTION_2_PSYS_EN BIT(11)
#define REG_ADAPTER_CURRENT 0x3F

// Bits 0-5, 13-15 are ignored.
#define CHARGE_CURRENT_MASK 0x1FC0

#if CHARGER_BATTERY_RSENSE == 5
    #define CHARGE_CURRENT (MIN((CHARGER_CHARGE_CURRENT >> 1), CHARGE_CURRENT_MASK) & CHARGE_CURRENT_MASK)
    // XXX: According to the datasheet, only 10 and 20 are valid.
    #define BATTERY_RSENSE (RSENSE_10 << 8)
#elif CHARGER_BATTERY_RSENSE == 10
    #define CHARGE_CURRENT (MIN(CHARGER_CHARGE_CURRENT, CHARGE_CURRENT_MASK) & CHARGE_CURRENT_MASK)
    #define BATTERY_RSENSE (RSENSE_10 << 8)
#elif CHARGER_BATTERY_RSENSE == 20
    #define CHARGE_CURRENT (MIN((CHARGER_CHARGE_CURRENT << 1), CHARGE_CURRENT_MASK) & CHARGE_CURRENT_MASK)
    #define BATTERY_RSENSE (RSENSE_20 << 8)
#else
    #error Invalid battery RSENSE value
#endif

// Bits 0-3, 15 are ignored.
#define CHARGE_VOLTAGE_MASK 0x7FF0
#define CHARGE_VOLTAGE (MIN(CHARGER_CHARGE_VOLTAGE, CHARGE_VOLTAGE_MASK) & CHARGE_VOLTAGE_MASK)

// Bits 0-6 are ignored. Bits 12-15 must be 0.
#define INPUT_CURRENT_MASK 0x0F80

#if CHARGER_ADAPTER_RSENSE == 5
    #define INPUT_CURRENT(X) (MIN(((X) >> 2), INPUT_CURRENT_MASK) & INPUT_CURRENT_MASK)
    #define ADAPTER_RSENSE RSENSE_5
#elif CHARGER_ADAPTER_RSENSE == 10
    #define INPUT_CURRENT(X) (MIN(((X) >> 1), INPUT_CURRENT_MASK) & INPUT_CURRENT_MASK)
    #define ADAPTER_RSENSE RSENSE_10
#elif CHARGER_ADAPTER_RSENSE == 20
    #define INPUT_CURRENT(X) (MIN((X), INPUT_CURRENT_MASK) & INPUT_CURRENT_MASK)
    #define ADAPTER_RSENSE RSENSE_20
#else
    #error Invalid adapter RSENSE value
#endif

// PSYS gain in nA/W
#ifndef CHARGER_PSYS_GAIN
    #define CHARGER_PSYS_GAIN 1000
#endif
#if CHARGER_PSYS_GAIN == 1000
    #define CHARGE_OPTION_2_PSYS_GAIN (0 << 8)
#elif CHARGER_PSYS_GAIN == 500
    #define CHARGE_OPTION_2_PSYS_GAIN (1 << 8)
#elif CHARGER_PSYS_GAIN == 250
    #define CHARGE_OPTION_2_PSYS_GAIN (2 << 8)
#elif CHARGER_PSYS_GAIN == 2000
    #define CHARGE_OPTION_2_PSYS_GAIN (3 << 8)
#else
    #error Invalid CHARGER_PSYS_GAIN value
#endif
// clang-format on

// Sense resistor values in milliohms.
enum sense_resistor {
    RSENSE_10 = 0,
    RSENSE_20 = 1,
    RSENSE_5 = 2,
};

// XXX: Assumption: ac_last is initialized high.
static bool charger_enabled = false;

int16_t battery_charger_disable(void) {
    int16_t res = 0;

    if (!charger_enabled)
        return 0;

    // Set charge option 1 to converter frequency 600 KHz and enable HPB
    //TODO: needed when charging disabled?
    res = smbus_write(
        CHARGER_ADDRESS,
        REG_CHARGE_OPTION_1,
        CHARGE_OPTION_1_600KHZ | ADAPTER_RSENSE | BATTERY_RSENSE | CHARGE_OPTION_1_HPB_EN
    );
    if (res < 0)
        return res;

    // Set charge option 2 to PSYS enable
    //TODO: needed when charging disabled?
    res = smbus_write(
        CHARGER_ADDRESS,
        REG_CHARGE_OPTION_2,
        CHARGE_OPTION_2_PSYS_EN | CHARGE_OPTION_2_PSYS_GAIN
    );
    if (res < 0)
        return res;

    // Disable charge current
    res = smbus_write(CHARGER_ADDRESS, REG_CHARGE_CURRENT, 0);
    if (res < 0)
        return res;

    // Disable charge voltage
    res = smbus_write(CHARGER_ADDRESS, REG_CHARGE_VOLTAGE, 0);
    if (res < 0)
        return res;

    // Disable input current
    res = smbus_write(CHARGER_ADDRESS, REG_ADAPTER_CURRENT, 0);
    if (res < 0)
        return res;

    DEBUG("Charger disabled\n");
    charger_enabled = false;
    return 0;
}

int16_t battery_charger_enable(void) {
    int16_t res = 0;

    if (charger_enabled)
        return 0;

    res = battery_charger_disable();
    if (res < 0)
        return res;

    // Set charge option 1 to converter frequency 600 KHz and enable HPB
    res = smbus_write(
        CHARGER_ADDRESS,
        REG_CHARGE_OPTION_1,
        CHARGE_OPTION_1_600KHZ | ADAPTER_RSENSE | BATTERY_RSENSE | CHARGE_OPTION_1_HPB_EN
    );
    if (res < 0)
        return res;

    // Set charge option 2 to PSYS enable
    res = smbus_write(
        CHARGER_ADDRESS,
        REG_CHARGE_OPTION_2,
        CHARGE_OPTION_2_PSYS_EN | CHARGE_OPTION_2_PSYS_GAIN
    );
    if (res < 0)
        return res;

    // Set charge current in mA
    res = smbus_write(CHARGER_ADDRESS, REG_CHARGE_CURRENT, CHARGE_CURRENT);
    if (res < 0)
        return res;

    // Set charge voltage in mV
    res = smbus_write(CHARGER_ADDRESS, REG_CHARGE_VOLTAGE, CHARGE_VOLTAGE);
    if (res < 0)
        return res;

    // Set input current in mA
    res = smbus_write(
        CHARGER_ADDRESS,
        REG_ADAPTER_CURRENT,
        INPUT_CURRENT(battery_charger_input_current)
    );
    if (res < 0)
        return res;

    DEBUG("Charger enabled\n");
    charger_enabled = true;
    return 0;
}

void battery_charger_event(void) {
    // Avoid watchdog timeout
    if (charger_enabled) {
        // Set charge voltage in mV
        smbus_write(CHARGER_ADDRESS, REG_CHARGE_VOLTAGE, CHARGE_VOLTAGE);
    }
}

void battery_debug(void) {
    uint16_t data = 0;
    int16_t res = 0;

#define command(N, A, V) \
    { \
        DEBUG("  " #N ": "); \
        res = smbus_read(A, V, &data); \
        if (res < 0) { \
            DEBUG("ERROR %04X\n", -res); \
        } else { \
            DEBUG("%04X\n", data); \
        } \
    }

    DEBUG("Battery:\n");
    command(Temperature, BATTERY_ADDRESS, 0x08);
    command(Voltage, BATTERY_ADDRESS, 0x09);
    command(Current, BATTERY_ADDRESS, 0x0A);
    command(Charge, BATTERY_ADDRESS, 0x0D);
    command(Status, BATTERY_ADDRESS, 0x16);

    DEBUG("Charger (oz26786):\n");
    command(ChargeCurrent, CHARGER_ADDRESS, REG_CHARGE_CURRENT);
    command(ChargeVoltage, CHARGER_ADDRESS, REG_CHARGE_VOLTAGE);
    command(ChargeOption_1, CHARGER_ADDRESS, REG_CHARGE_OPTION_1);
    command(ChargeOption_2, CHARGER_ADDRESS, REG_CHARGE_OPTION_2);
    command(AdapterCurrent, CHARGER_ADDRESS, REG_ADAPTER_CURRENT);

#undef command
}
