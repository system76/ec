// SPDX-License-Identifier: GPL-3.0-only

// USB-PD driver for TPS65987 and the mostly compatible TPS65993 and TPS65994.
// I2C register reference: https://www.ti.com/lit/ug/slvubh2b/slvubh2b.pdf

#include "usbpd.h"
#include <app/battery.h>
#include <app/power.h>
#include <board/gpio.h>
#include <common/debug.h>
#include <ec/i2c.h>

#define USBPD_ADDRESS 0x20

#define REG_ACTIVE_CONTRACT_PDO 0x34

void usbpd_init(void) {
    i2c_reset(&USBPD_I2C, true);
}

static int16_t usbpd_current_limit(void) {
    uint8_t value[7] = { 0 };
    int16_t res = i2c_get(&USBPD_I2C, USBPD_ADDRESS, REG_ACTIVE_CONTRACT_PDO, value, sizeof(value));
    if (res == 7) {
        if (value[0] == 6) {
            uint32_t pdo = ((uint32_t)value[1]) | (((uint32_t)value[2]) << 8) |
                (((uint32_t)value[3]) << 16) | (((uint32_t)value[4]) << 24);
            DEBUG("USBPD PDO %08lX ", pdo);
            uint8_t kind = (uint8_t)((pdo >> 30) & 0b11);
            if (kind == 0b00) {
                DEBUG("FIX ");
                uint32_t current_ma = (pdo & 0x3FF) * 10;
                DEBUG("%ld.%03ld A ", current_ma / 1000, current_ma % 1000);
                uint32_t voltage_mv = ((pdo >> 10) & 0x3FF) * 50;
                DEBUG("%ld.%03ld V\n", voltage_mv / 1000, voltage_mv % 1000);
                return (int16_t)current_ma;
            } else if (kind == 0b01) {
                DEBUG("BAT ");
                uint32_t power_mw = (pdo & 0x3FF) * 250;
                DEBUG("%ld.%03ld W ", power_mw / 1000, power_mw % 1000);
                uint32_t min_voltage_mv = ((pdo >> 10) & 0x3FF) * 50;
                DEBUG("%ld.%03ld Vmin ", min_voltage_mv / 1000, min_voltage_mv % 1000);
                uint32_t max_voltage_mv = ((pdo >> 20) & 0x3FF) * 50;
                DEBUG("%ld.%03ld Vax\n", max_voltage_mv / 1000, max_voltage_mv % 1000);
                //TODO
                return -0x5000;
            } else if (kind == 0b10) {
                DEBUG("VAR ");
                uint32_t current_ma = (pdo & 0x3FF) * 10;
                DEBUG("%ld.%03ld A ", current_ma / 1000, current_ma % 1000);
                uint32_t min_voltage_mv = ((pdo >> 10) & 0x3FF) * 50;
                DEBUG("%ld.%03ld Vmin ", min_voltage_mv / 1000, min_voltage_mv % 1000);
                uint32_t max_voltage_mv = ((pdo >> 20) & 0x3FF) * 50;
                DEBUG("%ld.%03ld Vax\n", max_voltage_mv / 1000, max_voltage_mv % 1000);
                return (int16_t)current_ma;
            } else {
                DEBUG("AUG\n");
                //TODO
                return -0x4000;
            }
        } else {
            return -(0x3000 | (int16_t)value[0]);
        }
    } else if (res < 0) {
        return res;
    } else {
        return -(0x2000 | res);
    }
}

static void usbpd_dump(void) {
    // Dump all registers for debugging
    for (uint8_t reg = 0x00; reg < 0x40; reg += 1) {
        uint8_t value[65] = { 0 };
        int16_t res = i2c_get(&USBPD_I2C, USBPD_ADDRESS, reg, value, sizeof(value));
        if (res < 0) {
            DEBUG("USBPD %02X ERROR %04X\n", reg, res);
        } else {
            DEBUG("USBPD %02X OK %04X %02X =", reg, res, value[0]);
            for (int i = 0; i < (int)value[0]; i++) {
                DEBUG(" %02X", value[i + 1]);
            }
            DEBUG("\n");
        }
    }
}

void usbpd_event(void) {
    bool update = false;

    static bool last_ac_in = false;
    bool ac_in = !gpio_get(&ACIN_N);
    if (ac_in != last_ac_in) {
        last_ac_in = ac_in;
        update = true;

        DEBUG("AC_IN %d\n", ac_in);
    }

    static bool last_jack_in = false;
    bool jack_in = !gpio_get(&JACK_IN_N);
    if (jack_in != last_jack_in) {
        last_jack_in = jack_in;
        update = true;

        DEBUG("JACK_IN %d\n", jack_in);
    }

    static bool last_sink_ctrl = false;
    bool sink_ctrl = gpio_get(&SINK_CTRL);
    if (sink_ctrl != last_sink_ctrl) {
        last_sink_ctrl = sink_ctrl;
        update = true;

        DEBUG("SINK_CTRL %d\n", sink_ctrl);
    }

    static enum PowerState last_power_state = POWER_STATE_OFF;
    update_power_state();
    if (power_state != last_power_state) {
        last_power_state = power_state;
        update = true;
    }

    if (update) {
        // Default to disabling input current
        uint16_t next_input_current = 0;

        if (ac_in) {
            if (jack_in) {
                // Use default input current
                next_input_current = CHARGER_INPUT_CURRENT;
            } else if (sink_ctrl) {
                int16_t res = usbpd_current_limit();
                if (res < 0) {
                    DEBUG("ERR %04X\n", -res);
                } else if (res < CHARGER_INPUT_CURRENT) {
                    // Use USB-PD charger current if it provides less than AC adapter
                    next_input_current = (uint16_t)res;
                } else {
                    // Use default input current if USB-PD charger provides more
                    next_input_current = CHARGER_INPUT_CURRENT;
                }
            }
        }

        if (sink_ctrl) {
            if (jack_in || power_state == POWER_STATE_S0) {
                usbpd_disable_charging();
            } else {
                usbpd_enable_charging();
            }
        }

        if (next_input_current != battery_charger_input_current) {
            battery_charger_input_current = next_input_current;
            DEBUG("CHARGER LIMIT %d mA\n", battery_charger_input_current);

            // Disable smart charger so it is reconfigured with the new limit
            battery_charger_disable();
        }
    }

    //TODO: dynamically set charger current (is this needed?)
}

static int16_t usbpd_aneg(void) {
    int16_t res;

    uint8_t cmd[5] = { 4, 'A', 'N', 'e', 'g' };
    res = i2c_set(&USBPD_I2C, USBPD_ADDRESS, 0x08, cmd, sizeof(cmd));
    if (res < 0) {
        return res;
    }

    //TODO: wait on command completion

    return 0;
}

void usbpd_disable_charging(void) {
    int16_t res;

    DEBUG("USBPD DISABLE CHARGING ");

    // Read current value
    uint8_t value[2] = { 0 };
    res = i2c_get(&USBPD_I2C, USBPD_ADDRESS, 0x33, value, sizeof(value));
    if (res < 0) {
        DEBUG("ERR %04X\n", -res);
        return;
    }

    // Charging already disabled
    if (value[1] == 1) {
        DEBUG("NOP\n");
        return;
    }

    // Enable only the first TX sink PDO (5V)
    value[0] = 1;
    value[1] = 1;
    res = i2c_set(&USBPD_I2C, USBPD_ADDRESS, 0x33, value, sizeof(value));
    if (res < 0) {
        DEBUG("ERR %04X\n", -res);
        return;
    }

    // Auto negotiate sink update
    res = usbpd_aneg();
    if (res < 0) {
        DEBUG("ERR %04X\n", -res);
        return;
    }

    DEBUG("OK\n");
}

void usbpd_enable_charging(void) {
    int16_t res;

    DEBUG("USBPD ENABLE CHARGING ");

    // Read current value
    uint8_t value[2] = { 0 };
    res = i2c_get(&USBPD_I2C, USBPD_ADDRESS, 0x33, value, sizeof(value));
    if (res < 0) {
        DEBUG("ERR %04X\n", -res);
        return;
    }

    // Charging already enabled
    if (value[1] == 2) {
        DEBUG("NOP\n");
        return;
    }

    // Enable the first two TX sink PDO (5V and 20V)
    value[0] = 1;
    value[1] = 2;
    res = i2c_set(&USBPD_I2C, USBPD_ADDRESS, 0x33, value, sizeof(value));
    if (res < 0) {
        DEBUG("ERR %04X\n", -res);
        return;
    }

    // Auto negotiate sink update
    res = usbpd_aneg();
    if (res < 0) {
        DEBUG("ERR %04X\n", -res);
        return;
    }

    DEBUG("OK\n");
}
