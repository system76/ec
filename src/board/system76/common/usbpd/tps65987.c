// SPDX-License-Identifier: GPL-3.0-only

// USB-PD driver for TPS65987 and the mostly compatible TPS65993 and TPS65994.
// I2C register reference: https://www.ti.com/lit/ug/slvubh2b/slvubh2b.pdf

#include <arch/time.h>
#include <board/battery.h>
#include <board/gpio.h>
#include <board/power.h>
#include <board/usbpd.h>
#include <common/debug.h>
#include <ec/i2c.h>

#define PORT_A_ADDRESS 0x20
#define PORT_B_ADDRESS 0x24

#define REG_MODE 0x03
#define REG_CMD1 0x08
#define REG_DATA1 0x09
#define REG_INT_EVENT_1 0x14
#define REG_INT_EVENT_2 0x15
#define REG_INT_CLEAR_1 0x18
#define REG_INT_CLEAR_2 0x19
#define REG_GLOBAL_CONFIG 0x27
#define REG_ACTIVE_CONTRACT_PDO 0x34

#ifndef HAVE_PD_IRQ
#define HAVE_PD_IRQ 0
#endif

enum {
    // PDO is empty
    USBPD_ERR_PDO_ZERO = 0x1000,
    // I2C error
    USBPD_ERR_I2C = 0x2000,
    // Incorrect register length
    USBPD_ERR_REG_LEN = 0x3000,
    // Unsupported PDO type was received
    USBPD_ERR_PDO_TYPE_UNSUP = 0x4000,
};

enum {
    // Fully functional, normal operation
    USBPD_MODE_APP = 0,
    // PD controller is running BIST
    USBPD_MODE_BIST,
    // PD controller booted in dead battery mode
    USBPD_MODE_BOOT,
    // Simulated port disconnect by previously issued DISC command
    USBPD_MODE_DISC,
    // Other values indicate limited functionality
    USBPD_MODE_UNKNOWN,
};

#define PDO_KIND(pdo) ((uint8_t)((pdo) >> 30 & 3))

enum {
    PDO_KIND_FIXED = 0,
    PDO_KIND_BATTERY,
    PDO_KIND_VARIABLE,
    PDO_KIND_AUGUMENTED,
};

#define PDO_CURRENT_MA(pdo) (((pdo) & 0x3FF) * 10)

static int16_t usbpd_current_limit(uint8_t address) {
    uint8_t value[7] = { 0 };
    int16_t res = i2c_get(&I2C_USBPD, address, REG_ACTIVE_CONTRACT_PDO, value, sizeof(value));
    if (res == 7) {
        if (value[0] == 6) {
            uint32_t pdo = ((uint32_t)value[1]) | (((uint32_t)value[2]) << 8) |
                (((uint32_t)value[3]) << 16) | (((uint32_t)value[4]) << 24);
            if (!pdo)
                return -USBPD_ERR_PDO_ZERO;
            DEBUG("USBPD PDO %08lX ", pdo);
            if (PDO_KIND(pdo) == PDO_KIND_FIXED) {
                DEBUG("FIX ");
                DEBUG("%ld.%03ld A\n", PDO_CURRENT_MA(pdo) / 1000, PDO_CURRENT_MA(pdo) % 1000);
                return (int16_t)PDO_CURRENT_MA(pdo);
            } else if (PDO_KIND(pdo) == PDO_KIND_BATTERY) {
                DEBUG("BAT\n");
                //TODO
                return -USBPD_ERR_PDO_TYPE_UNSUP;
            } else if (PDO_KIND(pdo) == PDO_KIND_VARIABLE) {
                DEBUG("VAR ");
                DEBUG("%ld.%03ld A\n", PDO_CURRENT_MA(pdo) / 1000, PDO_CURRENT_MA(pdo) % 1000);
                return (int16_t)PDO_CURRENT_MA(pdo);
            } else {
                DEBUG("AUG\n");
                //TODO
                return -USBPD_ERR_PDO_TYPE_UNSUP;
            }
        } else {
            return -(USBPD_ERR_REG_LEN | (int16_t)value[0]);
        }
    } else if (res < 0) {
        return res;
    } else {
        return -(USBPD_ERR_I2C | res);
    }
}

static void usbpd_dump(uint8_t address) {
    /* Dump all registers for debugging */
    for (uint8_t reg = 0x00; reg < 0x40; reg += 1) {
        uint8_t value[65] = { 0 };
        int16_t res = i2c_get(&I2C_USBPD, address, reg, value, sizeof(value));
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

#define FOURCC(a, b, c, d) (((((d) << 8) | (c)) << 8 | (b)) << 8 | (a))

// Check the operational state of the PD controller
static int16_t usbpd_get_mode(void) {
    int16_t res;
    int16_t mode;
    uint8_t reg[5] = { 0 };

    DEBUG("USBPD controller mode: ");

    res = i2c_get(&I2C_USBPD, PORT_A_ADDRESS, REG_MODE, reg, sizeof(reg));
    if (res < 0 || reg[0] < 4) {
        DEBUG("UNKNOWN (I2C error %d)\n", res);
        return USBPD_MODE_UNKNOWN;
    }

    mode = FOURCC(reg[1], reg[2], reg[3], reg[4]);

    switch (mode) {
    case FOURCC('A', 'P', 'P', ' '):
        DEBUG("APP\n");
        return USBPD_MODE_APP;
    case FOURCC('B', 'I', 'S', 'T'):
        DEBUG("BIST\n");
        return USBPD_MODE_BIST;
    case FOURCC('B', 'O', 'O', 'T'):
        DEBUG("BOOT\n");
        return USBPD_MODE_BOOT;
    case FOURCC('D', 'I', 'S', 'C'):
        DEBUG("DISC\n");
        return USBPD_MODE_DISC;
    }

    DEBUG("UNKNOWN %llx\n", mode);
    return USBPD_MODE_UNKNOWN;
}

// Return to normal operation
// Reboots the PD controller and exits any modal tasks
void usbpd_reset(void) {
    int16_t res;

    uint8_t cmd[5] = { 4, 'G', 'a', 'i', 'd' };

    res = i2c_set(&I2C_USBPD, PORT_A_ADDRESS, REG_CMD1, cmd, sizeof(cmd));
    if (res < 0)
        return;

    i2c_reset(&I2C_USBPD, true);
    return;
}

// Clear dead battery flag
// Switches the PD controller power supply from Vbus to VIN_3V3
static void usbpd_dbfg(void) {
    int16_t res;

    uint8_t cmd[5] = { 4, 'D', 'B', 'f', 'g' };

    res = i2c_set(&I2C_USBPD, PORT_A_ADDRESS, REG_CMD1, cmd, sizeof(cmd));
    if (res < 0)
        return;

    i2c_reset(&I2C_USBPD, true);
    return;
}

// Enables automatic sink management policy
// Port with highest negotiated power will be used
static void usbpd_set_multiport_policy(void) {
    int16_t res;
    uint8_t reg[15] = { 0 };

    DEBUG("USBPD set multiport policy\n");
    res = i2c_get(&I2C_USBPD, PORT_A_ADDRESS, REG_GLOBAL_CONFIG, reg, sizeof(reg));
    if (res < 0)
        return;

    DEBUG("USBPD multiport policy: %x\n", reg[4] & 0x01);
    if (reg[4] & 0x01)
        return;

    reg[4] |= 0x01; // Highest Power sink policy

    res = i2c_set(&I2C_USBPD, PORT_A_ADDRESS, REG_GLOBAL_CONFIG, reg, sizeof(reg));
    DEBUG("USBPD multiport policy set RES = %ld\n", res);
}

static void usbpd_clear_event() {
    int16_t res;
    uint8_t reg[12] = { 0 };

    DEBUG("USBPD IRQ\n");

    res = i2c_get(&I2C_USBPD, PORT_A_ADDRESS, REG_INT_EVENT_1, reg, sizeof(reg));
    if (res < 0)
        return;

    res = i2c_set(&I2C_USBPD, PORT_A_ADDRESS, REG_INT_CLEAR_1, reg, sizeof(reg));
    if (res < 0)
        return;

#ifdef USBPD_DUAL_PORT
    res = i2c_get(&I2C_USBPD, PORT_B_ADDRESS, REG_INT_EVENT_1, reg, sizeof(reg));
    if (res < 0)
        return;

    res = i2c_set(&I2C_USBPD, PORT_B_ADDRESS, REG_INT_CLEAR_1, reg, sizeof(reg));
    if (res < 0)
        return;
#endif
}

void usbpd_event(void) {
    bool update = false;
    int16_t res;

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

    static bool last_sink_ctrl_1 = false;
    bool sink_ctrl_1 = gpio_get(&SINK_CTRL);
    if (sink_ctrl_1 != last_sink_ctrl_1) {
        last_sink_ctrl_1 = sink_ctrl_1;
        update = true;

        DEBUG("SINK_CTRL %d\n", sink_ctrl_1);
    }

#ifdef USBPD_DUAL_PORT
    static bool last_sink_ctrl_2 = false;
    bool sink_ctrl_2 = gpio_get(&SINK_CTRL_2);
    if (sink_ctrl_2 != last_sink_ctrl_2) {
        last_sink_ctrl_2 = sink_ctrl_2;
        update = true;

        DEBUG("SINK_CTRL_2 %d\n", sink_ctrl_2);
    }
#endif

    static enum PowerState last_power_state = POWER_STATE_OFF;
    update_power_state();
    if (power_state != last_power_state) {
        update = true;

        if (last_power_state == POWER_STATE_OFF) {
            // VIN_3V3 now available, allow PD to use it instead of Vbus
            usbpd_dbfg();
#ifdef USBPD_DUAL_PORT
            // This resets the PD port so it has to be done after dbfg
            usbpd_set_multiport_policy();
#endif
        }
        last_power_state = power_state;
    }

    if (update) {
        // Default to disabling input current
        uint16_t next_input_current = 0;
        uint16_t next_input_voltage = 0;
        uint16_t retry = 1000;

        if (ac_in) {
            if (jack_in) {
                // Use default input current
                next_input_current = CHARGER_INPUT_CURRENT;
                next_input_voltage = BATTERY_CHARGER_VOLTAGE_AC;
            } else if (sink_ctrl_1) {
                while ((res = usbpd_current_limit(PORT_A_ADDRESS)) < 0 && retry--) {};
                next_input_current = res < CHARGER_INPUT_CURRENT ? res : CHARGER_INPUT_CURRENT;
                next_input_current = (uint32_t)next_input_current * 85 / 100;
                next_input_voltage = BATTERY_CHARGER_VOLTAGE_PD;
#ifdef USBPD_DUAL_PORT
            } else if (sink_ctrl_2) {
                while ((res = usbpd_current_limit(PORT_B_ADDRESS)) < 0 && retry--) {};
                next_input_current = res < CHARGER_INPUT_CURRENT ? res : CHARGER_INPUT_CURRENT;
                next_input_current = (uint32_t)next_input_current * 85 / 100;
                next_input_voltage = BATTERY_CHARGER_VOLTAGE_PD;
#endif
            }
        }

        if (next_input_current != battery_charger_input_current_ma) {
            battery_charger_input_current_ma = next_input_current;
            battery_charger_input_voltage_v = next_input_voltage;
            DEBUG("CHARGER LIMIT %d mA\n", battery_charger_input_current_ma);

            // Disable smart charger so it is reconfigured with the new limit
            battery_charger_disable();
            // In case power was renegotiated without power loss
            power_apply_limit(true);
        }
    }

#if HAVE_PD_IRQ
    /* For now, all we do is clear all events */
    if (power_state != POWER_STATE_OFF && !gpio_get(&PD_IRQ))
        usbpd_clear_event();
#endif
}

void usbpd_init(void) {
    i2c_reset(&I2C_USBPD, true);
}
