// SPDX-License-Identifier: GPL-3.0-only

// USB-PD driver for TPS65987 and the mostly compatible TPS65993 and TPS65994.
// I2C register reference: https://www.ti.com/lit/ug/slvubh2b/slvubh2b.pdf

#include <board/battery.h>
#include <board/usbpd.h>
#include <common/debug.h>
#include <ec/i2c.h>

#define USBPD_ADDRESS 0x20

#define REG_ACTIVE_CONTRACT_PDO 0x34

void usbpd_init(void) {
    i2c_reset(&I2C_USBPD, true);
}

int16_t usbpd_current_limit(void) {
    uint8_t value[7] = { 0 };
    int16_t res = i2c_get(&I2C_USBPD, USBPD_ADDRESS, REG_ACTIVE_CONTRACT_PDO, value, 7);
    if (res == 7) {
        if (value[0] == 6) {
            uint32_t pdo =
                ((uint32_t)value[1]) |
                (((uint32_t)value[2]) << 8) |
                (((uint32_t)value[3]) << 16) |
                (((uint32_t)value[4]) << 24);
            TRACE("USBPD PDO %08lX\n", pdo);
            uint8_t kind = (uint8_t)((pdo >> 30) & 0b11);
            if (kind == 0b00) {
                TRACE(" FIX");
                uint32_t current_ma = (pdo & 0x3FF) * 10;
                TRACE(" %ld.%03ld A", current_ma / 1000, current_ma % 1000);
                uint32_t voltage_mv = ((pdo >> 10) & 0x3FF) * 50;
                TRACE(" %ld.%03ld V", voltage_mv / 1000, voltage_mv % 1000);
                return (int16_t)current_ma;
            } else if (kind == 0b01) {
                TRACE(" BAT");
                uint32_t power_mw = (pdo & 0x3FF) * 250;
                TRACE(" %ld.%03ld W", power_mw / 1000, power_mw % 1000);
                uint32_t min_voltage_mv = ((pdo >> 10) & 0x3FF) * 50;
                TRACE(" %ld.%03ld Vmin", min_voltage_mv / 1000, min_voltage_mv % 1000);
                uint32_t max_voltage_mv = ((pdo >> 20) & 0x3FF) * 50;
                TRACE(" %ld.%03ld Vax", max_voltage_mv / 1000, max_voltage_mv % 1000);
                //TODO
                return -0x5000;
            } else if (kind == 0b10) {
                TRACE(" VAR");
                uint32_t current_ma = (pdo & 0x3FF) * 10;
                TRACE(" %ld.%03ld A", current_ma / 1000, current_ma % 1000);
                uint32_t min_voltage_mv = ((pdo >> 10) & 0x3FF) * 50;
                TRACE(" %ld.%03ld Vmin", min_voltage_mv / 1000, min_voltage_mv % 1000);
                uint32_t max_voltage_mv = ((pdo >> 20) & 0x3FF) * 50;
                TRACE(" %ld.%03ld Vax", max_voltage_mv / 1000, max_voltage_mv % 1000);
                return (int16_t)current_ma;
            } else {
                TRACE(" AUG");
                //TODO
                return -0x4000;
            }
            TRACE("\n");
        } else {
            return -(0x3000 | (int16_t)value[0]);
        }
    } else if (res < 0) {
        return res;
    } else {
        return -(0x2000 | res);
    }
}

void usbpd_event(void) {
    /* Dump all registers for debugging
    for(uint8_t reg = 0x00; reg < 0x80; reg+=1) {
        uint8_t value[65] = { 0 };
        int16_t res = i2c_get(&I2C_DGPU, USBPD_ADDRESS, reg, value, 65);
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
    */

    DEBUG("USBPD LIMIT ");
    int16_t res = usbpd_current_limit();
    if (res < 0) {
        DEBUG("ERR %04X\n", -res);
        //TODO: determine if USB-PD is actually plugged (SINK_CTRL_EC)
        battery_charger_input_current = CHARGER_INPUT_CURRENT;
    } else {
        if (res < CHARGER_INPUT_CURRENT) {
            // Use USB-PD charger current if it provides less than AC adapter
            battery_charger_input_current = (uint16_t)res;
        } else {
            // Use AC adapter current if USB-PD charger supports more
            battery_charger_input_current = CHARGER_INPUT_CURRENT;
        }
        DEBUG("%d mA\n", battery_charger_input_current);
    }

    //TODO: dynamically set charger current (is this needed?)
}
