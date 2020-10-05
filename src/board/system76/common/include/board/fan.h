// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_FAN_H
#define _BOARD_FAN_H

#include <stdbool.h>
#include <stdint.h>

#define PWM_DUTY(X) ((uint8_t)(((((uint16_t)(X)) * 255) + 99) / 100))

struct FanPoint {
    int16_t temp;
    uint8_t duty;
};

struct Fan {
    const struct FanPoint * points;
    uint8_t points_size;
    uint8_t * heatup;
    uint8_t heatup_size;
    uint8_t * cooldown;
    uint8_t cooldown_size;
    bool interpolate;
};

extern bool fan_max;

void fan_reset(void);

uint8_t fan_duty(const struct Fan * fan, int16_t temp) __reentrant;
uint8_t fan_heatup(const struct Fan * fan, uint8_t duty) __reentrant;
uint8_t fan_cooldown(const struct Fan * fan, uint8_t duty) __reentrant;

#endif // _BOARD_FAN_H
