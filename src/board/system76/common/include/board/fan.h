// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_FAN_H
#define _BOARD_FAN_H

#include <stdbool.h>
#include <stdint.h>

struct FanPoint {
    int16_t temp;
    uint8_t duty;
};

#define PWM_DUTY(X) ((uint8_t)(((((uint16_t)(X)) * 255) + 99) / 100))

extern bool fan_max;

void fan_reset(void);

#endif // _BOARD_FAN_H
