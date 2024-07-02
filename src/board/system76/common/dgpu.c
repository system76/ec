// SPDX-License-Identifier: GPL-3.0-only

#include <board/dgpu.h>
#include <board/gpio.h>
#include <board/power.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/i2c.h>

int16_t dgpu_temp = 0;

void dgpu_init(void) {
    // Set up for i2c usage
    i2c_reset(&I2C_DGPU, true);
}

bool dgpu_get_temp(int16_t *const data) {
    if (gpio_get(&DGPU_PWR_EN) && !gpio_get(&GC6_FB_EN)) {
        int8_t rlts;
        int16_t res = i2c_get(&I2C_DGPU, 0x4F, 0x00, &rlts, 1);
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
    if (power_state == POWER_STATE_S0) {
        if (dgpu_get_temp(&dgpu_temp)) {
            return;
        }
    }

    dgpu_temp = 0;
}
