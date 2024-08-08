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
    const struct FanPoint *points;
    uint8_t points_size;
    uint8_t *heatup;
    uint8_t heatup_size;
    uint8_t *cooldown;
    uint8_t cooldown_size;
    uint8_t pwm_min;
};

extern bool fan_max;

extern uint8_t fan1_pwm_actual;
extern uint8_t fan1_pwm_target;
extern uint16_t fan1_rpm;
extern uint8_t fan2_pwm_actual;
extern uint8_t fan2_pwm_target;
extern uint16_t fan2_rpm;

void fan_reset(void);
void fan_event(void);

#endif // _BOARD_FAN_H
