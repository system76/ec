// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_FAN_H
#define _BOARD_FAN_H

#include <stdbool.h>
#include <stdint.h>

#define PWM_DUTY(X) ((uint8_t)(((((uint16_t)(X)) * 255) + 99) / 100))

struct FanPoint {
    const int16_t temp;
    const uint8_t duty;
};

#define FAN_POINT(T, D) { .temp = (int16_t)(T), .duty = PWM_DUTY(D) }

struct Fan {
    const struct FanPoint *const points;
    const uint8_t points_size;
    const uint8_t pwm_min;
};

extern const struct Fan __code FAN1;
#ifdef FAN2_PWM
extern const struct Fan __code FAN2;
#endif

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
