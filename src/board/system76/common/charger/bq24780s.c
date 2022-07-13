// SPDX-License-Identifier: GPL-3.0-only
//
// TI BQ24780S Smart Charger
// https://www.ti.com/lit/ds/symlink/bq24780s.pdf

#include <board/battery.h>
#include <board/smbus.h>
#include <common/macro.h>
#include <common/debug.h>

// Registers
#define REG_CHARGE_CURRENT 0x14
#define REG_CHARGE_VOLTAGE 0x15
#define REG_DISCHARGE_CURRENT 0x39
#define REG_INPUT_CURRENT 0x3F

// ChargeOption0 flags
// Low Power Mode Enable
#define SBC_EN_LWPWR        ((uint16_t)(BIT(15)))
// Watchdog Timer Adjust
#define SBC_WDTMR_ADJ_175S  ((uint16_t)(0b11 << 13))
// Switching Frequency
#define SBC_PWM_FREQ_800KHZ ((uint16_t)(0b01 << 8))
// IDCHG Amplifier Gain
#define SBC_IDCHC_GAIN      ((uint16_t)(BIT(3)))

// Bits 0-5 are ignored. Bits 13-15 must be 0.
#define CHARGE_CURRENT_MASK 0x1FC0

#if CHARGER_BATTERY_RSENSE == 5
    #define CHARGE_CURRENT ((CHARGER_CHARGE_CURRENT >> 1) & CHARGE_CURRENT_MASK)
#elif CHARGER_BATTERY_RSENSE == 10
    #define CHARGE_CURRENT (CHARGER_CHARGE_CURRENT & CHARGE_CURRENT_MASK)
#elif CHARGER_BATTERY_RSENSE == 20
    #define CHARGE_CURRENT ((CHARGER_CHARGE_CURRENT << 1) & CHARGE_CURRENT_MASK)
#else
    #error Invalid battery RSENSE value
#endif

// Bits 0-3, 15 are ignored.
#define CHARGE_VOLTAGE_MASK 0x7FF0
#define CHARGE_VOLTAGE (CHARGER_CHARGE_VOLTAGE & CHARGE_VOLTAGE_MASK)

// Bits 0-6 are ignored. Bits 12-15 must be 0.
#define INPUT_CURRENT_MASK 0x0F80

#if CHARGER_ADAPTER_RSENSE == 5
    #define INPUT_CURRENT ((CHARGER_INPUT_CURRENT >> 1) & INPUT_CURRENT_MASK)
#elif CHARGER_ADAPTER_RSENSE == 10
    #define INPUT_CURRENT (CHARGER_INPUT_CURRENT & INPUT_CURRENT_MASK)
#elif CHARGER_ADAPTER_RSENSE == 20
    #define INPUT_CURRENT ((CHARGER_INPUT_CURRENT << 1) & INPUT_CURRENT_MASK)
#else
    #error Invalid adapter RSENSE value
#endif

// XXX: Assumption: ac_last is initialized high.
static bool charger_enabled = false;

int16_t battery_charger_disable(void) {
    int16_t res = 0;

    if (!charger_enabled) return 0;

    // Set charge option 0 with 175s watchdog
    res = smbus_write(
        CHARGER_ADDRESS,
        0x12,
        SBC_EN_LWPWR |
        SBC_WDTMR_ADJ_175S |
        SBC_PWM_FREQ_800KHZ |
        SBC_IDCHC_GAIN
    );

    // Disable charge current
    res = smbus_write(CHARGER_ADDRESS, REG_CHARGE_CURRENT, 0);
    if (res < 0) return res;

    // Disable charge voltage
    res = smbus_write(CHARGER_ADDRESS, REG_CHARGE_VOLTAGE, 0);
    if (res < 0) return res;

    // Disable input current
    res = smbus_write(CHARGER_ADDRESS, REG_INPUT_CURRENT, 0);
    if (res < 0) return res;

    DEBUG("Charger disabled\n");
    charger_enabled = false;
    return 0;
}

int16_t battery_charger_enable(void) {
    int16_t res = 0;

    if (charger_enabled) return 0;

    res = battery_charger_disable();
    if (res < 0) return res;

    // Set charge current in mA
    res = smbus_write(CHARGER_ADDRESS, REG_CHARGE_CURRENT, CHARGE_CURRENT);
    if (res < 0) return res;

    // Set charge voltage in mV
    res = smbus_write(CHARGER_ADDRESS, REG_CHARGE_VOLTAGE, CHARGE_VOLTAGE);
    if (res < 0) return res;

    // Set input current in mA
    res = smbus_write(CHARGER_ADDRESS, REG_INPUT_CURRENT, INPUT_CURRENT);
    if (res < 0) return res;

    // Set charge option 0 with watchdog disabled
    res = smbus_write(
        CHARGER_ADDRESS,
        0x12,
        SBC_EN_LWPWR |
        SBC_PWM_FREQ_800KHZ |
        SBC_IDCHC_GAIN
    );

    DEBUG("Charger enabled\n");
    charger_enabled = true;
    return 0;
}

void battery_charger_event(void) {
    //TODO: watchdog
}

void battery_debug(void) {
    uint16_t data = 0;
    int16_t res = 0;

    #define command(N, A, V) { \
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

    DEBUG("Charger (bq24780s):\n");
    command(ChargeOption0, CHARGER_ADDRESS, 0x12);
    command(ChargeOption1, CHARGER_ADDRESS, 0x3B);
    command(ChargeOption2, CHARGER_ADDRESS, 0x38);
    command(ChargeOption3, CHARGER_ADDRESS, 0x37);
    command(ChargeCurrent, CHARGER_ADDRESS, REG_CHARGE_CURRENT);
    command(ChargeVoltage, CHARGER_ADDRESS, REG_CHARGE_VOLTAGE);
    command(DishargeCurrent, CHARGER_ADDRESS, REG_DISCHARGE_CURRENT);
    command(InputCurrent, CHARGER_ADDRESS, REG_INPUT_CURRENT);
    command(ProchotOption0, CHARGER_ADDRESS, 0x3C);
    command(ProchotOption1, CHARGER_ADDRESS, 0x3D);
    command(ProchotStatus, CHARGER_ADDRESS, 0x3A);

    #undef command
}
