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

struct FanInfo {
    uint8_t pwm_actual;
    uint8_t pwm_target;
    uint16_t rpm;
};

extern const struct Fan __code FAN1;
extern struct FanInfo fan1_info;

#ifdef FAN2_PWM
extern const struct Fan __code FAN2;
extern struct FanInfo fan2_info;
#endif

extern bool fan_max;

void fan_reset(void);
void fan_event(void);

#endif // _BOARD_FAN_H
