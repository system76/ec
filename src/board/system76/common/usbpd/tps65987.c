// SPDX-License-Identifier: GPL-3.0-only

// USB-PD driver for TPS65987 and the mostly compatible TPS65993 and TPS65994.
// I2C register reference: https://www.ti.com/lit/ug/slvubh2b/slvubh2b.pdf

#include <board/usbpd.h>
#include <board/battery.h>
#include <board/gpio.h>
#include <board/power.h>
#include <common/command.h>
#include <common/debug.h>
#include <ec/i2c.h>

#define USBPD_ADDRESS 0x20

#define REG_CMD_1 0x08
#define REG_TX_SINK_CAPABILITIES 0x33
#define REG_ACTIVE_CONTRACT_PDO 0x34
#define REG_DATA_STATUS 0x5F
    #define REG_DATA_STATUS_DATA_ORIENTATION BIT(1)
    #define REG_DATA_STATUS_ACTIVE_CABLE BIT(2)
    #define REG_DATA_STATUS_USB3_CONNECTION BIT(5)
    #define REG_DATA_STATUS_USB_DATA_ROLE BIT(7)
    #define REG_DATA_STATUS_DP_CONNECTION BIT(8)
    #define REG_DATA_STATUS_DP_PIN_ASSIGNMENT_MASK (0b11 << 10)
    #define REG_DATA_STATUS_DP_PIN_ASSIGNMENT_AB (0b10 << 10)
    #define REG_DATA_STATUS_DP_PIN_ASSIGNMENT_CD (0b01 << 10)
    #define REG_DATA_STATUS_DP_PIN_ASSIGNMENT_EF (0b00 << 10)
    #define REG_DATA_STATUS_DEBUG_ACCESSORY_MODE BIT(12)
    #define REG_DATA_STATUS_HPD_IRQ BIT(14)
    #define REG_DATA_STATUS_HPD_LEVEL BIT(15)

#ifndef HAVE_USBPD_CHARGING
#define HAVE_USBPD_CHARGING 0
#endif // HAVE_USBPD_CHARGING

void usbpd_init(void) {
    i2c_reset(&I2C_USBPD, true);
}

#if HAVE_USBPD_CHARGING

static int16_t usbpd_current_limit(void) {
    uint8_t value[7] = { 0 };
    int16_t res = i2c_get(&I2C_USBPD, USBPD_ADDRESS, REG_ACTIVE_CONTRACT_PDO, value, sizeof(value));
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
        int16_t res = i2c_get(&I2C_USBPD, USBPD_ADDRESS, reg, value, sizeof(value));
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
    res = i2c_set(&I2C_USBPD, USBPD_ADDRESS, REG_CMD_1, cmd, sizeof(cmd));
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
    res = i2c_get(&I2C_USBPD, USBPD_ADDRESS, REG_TX_SINK_CAPABILITIES, value, sizeof(value));
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
    res = i2c_set(&I2C_USBPD, USBPD_ADDRESS, REG_TX_SINK_CAPABILITIES, value, sizeof(value));
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
    res = i2c_get(&I2C_USBPD, USBPD_ADDRESS, REG_TX_SINK_CAPABILITIES, value, sizeof(value));
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
    res = i2c_set(&I2C_USBPD, USBPD_ADDRESS, REG_TX_SINK_CAPABILITIES, value, sizeof(value));
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

#else // HAVE_USBPD_CHARGING

void usbpd_event(void) {
    bool update = false;

    static bool last_ac_in = false;
    bool ac_in = !gpio_get(&ACIN_N);
    if (ac_in != last_ac_in) {
        last_ac_in = ac_in;
        update = true;

        DEBUG("AC_IN %d\n", ac_in);
    }

    if (update) {
        int16_t res;

        DEBUG("USBPD DATA STATUS ");

        // Read current value
        uint8_t value[6] = { 0 };
        res = i2c_get(&I2C_USBPD, USBPD_ADDRESS, REG_DATA_STATUS, value, sizeof(value));
        if (res < 0) {
            DEBUG("ERR %04X\n", -res);
        } else {
            DEBUG(
                "OK %02x = %02X, %02X%02X%02X%02X\n",
                value[0],
                value[5],
                value[4],
                value[3],
                value[2],
                value[1]
            );
        }
    }
}

void usbpd_disable_charging(void) {}

#endif // HAVE_USBPD_CHARGING

bool usbc_mux_info(uint8_t port, uint16_t *info) {
    if (port != 0) {
        // Only port 0 is supported right now
        WARN("usbc_mux_info does not support port %d\n", port);
        return false;
    }

    uint8_t value[6] = { 0 };
    int16_t res = i2c_get(&I2C_USBPD, USBPD_ADDRESS, REG_DATA_STATUS, value, sizeof(value));
    if (res < 0) {
        DEBUG("ERR %04X\n", -res);
        return false;
    }

    uint32_t data_status = ((uint32_t)value[1]) | (((uint32_t)value[2]) << 8) |
        (((uint32_t)value[3]) << 16) | (((uint32_t)value[4]) << 24);

    DEBUG("OK %02X, %02x = %02X, %08X\n", res, value[0], value[5], data_status);

    *info = 0;
    if (data_status & REG_DATA_STATUS_DP_CONNECTION) {
        *info |= CMD_USBC_MUX_INFO_DP;
    }
    if (data_status & REG_DATA_STATUS_USB3_CONNECTION) {
        *info |= CMD_USBC_MUX_INFO_USB;
    }
    if (data_status & REG_DATA_STATUS_ACTIVE_CABLE) {
        *info |= CMD_USBC_MUX_INFO_CABLE;
    }
    if (data_status & REG_DATA_STATUS_DATA_ORIENTATION) {
        *info |= CMD_USBC_MUX_INFO_POLARITY;
    }
    if (data_status & REG_DATA_STATUS_HPD_LEVEL) {
        *info |= CMD_USBC_MUX_INFO_HPD_LVL;
    }
    if (data_status & REG_DATA_STATUS_HPD_IRQ) {
        *info |= CMD_USBC_MUX_INFO_HPD_IRQ;
    }
    if (data_status & REG_DATA_STATUS_USB_DATA_ROLE) {
        *info |= CMD_USBC_MUX_INFO_UFP;
    }
    if (data_status & REG_DATA_STATUS_DEBUG_ACCESSORY_MODE) {
        *info |= CMD_USBC_MUX_INFO_DBG_ACC;
    }

    switch (data_status & REG_DATA_STATUS_DP_PIN_ASSIGNMENT_MASK) {
    case REG_DATA_STATUS_DP_PIN_ASSIGNMENT_AB:
        if (data_status & REG_DATA_STATUS_USB3_CONNECTION) {
            *info |= CMD_USBC_MUX_INFO_DP_MODE_B;
        } else {
            *info |= CMD_USBC_MUX_INFO_DP_MODE_A;
        }
        break;
    case REG_DATA_STATUS_DP_PIN_ASSIGNMENT_CD:
        if (data_status & REG_DATA_STATUS_USB3_CONNECTION) {
            *info |= CMD_USBC_MUX_INFO_DP_MODE_D;
        } else {
            *info |= CMD_USBC_MUX_INFO_DP_MODE_C;
        }
        break;
    case REG_DATA_STATUS_DP_PIN_ASSIGNMENT_EF:
        if (data_status & REG_DATA_STATUS_USB3_CONNECTION) {
            *info |= CMD_USBC_MUX_INFO_DP_MODE_F;
        } else {
            *info |= CMD_USBC_MUX_INFO_DP_MODE_E;
        }
        break;
    }

    DEBUG("USBC_MUX_INFO: %04X\n", *info);
    return true;
}
