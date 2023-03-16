// SPDX-License-Identifier: GPL-3.0-only

// USB-PD driver for TPS65987 and the mostly compatible TPS65993 and TPS65994.
// I2C register reference: https://www.ti.com/lit/ug/slvubh2b/slvubh2b.pdf

#include <board/usbpd.h>
#include <common/debug.h>
#include <ec/i2c.h>

#define USBPD_ADDRESS 0x20

#define REG_ACTIVE_CONTRACT_PDO 0x34

void usbpd_init(void) {
    i2c_reset(&I2C_USBPD, true);
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

    uint8_t value[7] = { 0 };
    int16_t res = i2c_get(&I2C_USBPD, USBPD_ADDRESS, REG_ACTIVE_CONTRACT_PDO, value, 7);
    if (res == 7) {
        if (value[0] == 6) {
            uint32_t pdo =
                ((uint32_t)value[1]) |
                (((uint32_t)value[2]) << 8) |
                (((uint32_t)value[3]) << 16) |
                (((uint32_t)value[4]) << 24);
            DEBUG("USBPD PDO %08lX\n", pdo);
            uint8_t kind = (uint8_t)((pdo >> 30) & 0b11);
            if (kind == 0b00) {
                DEBUG(" FIX");
                uint32_t current_ma = (pdo & 0x3FF) * 10;
                DEBUG(" %ld.%03ld A", current_ma / 1000, current_ma % 1000);
                uint32_t voltage_mv = ((pdo >> 10) & 0x3FF) * 50;
                DEBUG(" %ld.%03ld V", voltage_mv / 1000, voltage_mv % 1000);
            } else if (kind == 0b01) {
                DEBUG(" BAT");
                uint32_t power_mw = (pdo & 0x3FF) * 250;
                DEBUG(" %ld.%03ld W", power_mw / 1000, power_mw % 1000);
                uint32_t min_voltage_mv = ((pdo >> 10) & 0x3FF) * 50;
                DEBUG(" %ld.%03ld Vmin", min_voltage_mv / 1000, min_voltage_mv % 1000);
                uint32_t max_voltage_mv = ((pdo >> 20) & 0x3FF) * 50;
                DEBUG(" %ld.%03ld Vax", max_voltage_mv / 1000, max_voltage_mv % 1000);
            } else if (kind == 0b10) {
                DEBUG(" VAR");
                uint32_t current_ma = (pdo & 0x3FF) * 10;
                DEBUG(" %ld.%03ld A", current_ma / 1000, current_ma % 1000);
                uint32_t min_voltage_mv = ((pdo >> 10) & 0x3FF) * 50;
                DEBUG(" %ld.%03ld Vmin", min_voltage_mv / 1000, min_voltage_mv % 1000);
                uint32_t max_voltage_mv = ((pdo >> 20) & 0x3FF) * 50;
                DEBUG(" %ld.%03ld Vax", max_voltage_mv / 1000, max_voltage_mv % 1000);
            } else {
                //TODO
                DEBUG(" AUG");
            }
            DEBUG("\n");
        } else {
            DEBUG("USBPD PDO LEN %d\n", value[0]);
        }
    } else if (res < 0) {
        DEBUG("USBPD ERR %04X\n", -res);
    } else {
        DEBUG("USBPD I2C LEN %d\n", res);
    }
}
