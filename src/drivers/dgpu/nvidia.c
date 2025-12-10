// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

#include "dgpu.h"
#include <app/power.h>
#include <board/gpio.h>
#include <common/debug.h>
#include <common/macro.h>
#include <soc/i2c.h>

int16_t dgpu_temp = 0;

// Update interval is 250ms, so average over 1s period
static int16_t dgpu_temps[4] = { 0 };

void dgpu_init(void) {
    // Set up for i2c usage
    i2c_reset(&DGPU_I2C, true);
}

bool dgpu_get_temp(int16_t *const data) {
    if (gpio_get(&DGPU_PWR_EN) && !gpio_get(&GC6_FB_EN)) {
        int8_t rlts;
        int16_t res = i2c_get(&DGPU_I2C, 0x4F, 0x00, &rlts, 1);
        if (res == 1) {
            *data = (int16_t)rlts;
            return true;
        } else {
            DEBUG("DGPU temp error: %d\n", res);
            *data = 0;
            return false;
        }
    }

    *data = 0;
    return true;
}

void dgpu_read_temp(void) {
    dgpu_temps[0] = dgpu_temps[1];
    dgpu_temps[1] = dgpu_temps[2];
    dgpu_temps[2] = dgpu_temps[3];

    if (power_state == POWER_STATE_S0) {
        if (!dgpu_get_temp(&dgpu_temps[3])) {
            dgpu_temps[3] = 0;
        }
    } else {
        dgpu_temps[3] = 0;
    }

    dgpu_temp = (dgpu_temps[0] + dgpu_temps[1] + dgpu_temps[2] + dgpu_temps[3]) /
        ARRAY_SIZE(dgpu_temps);
}
