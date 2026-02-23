// SPDX-License-Identifier: GPL-3.0-only

#ifndef _APP_FAN_H
#define _APP_FAN_H

#include <stdbool.h>
#include <stdint.h>

struct FanPoint {
    int16_t temp;
    uint8_t duty;
};

#define PWM_DUTY(X) ((uint8_t)(((((uint16_t)(X)) * 255) + 99) / 100))
#define FAN_POINT(T, D) { .temp = (int16_t)(T), .duty = PWM_DUTY(D) }

struct Fan {
    const struct FanPoint *points;
    uint8_t points_size;
    uint8_t *heatup;
    uint8_t heatup_size;
    uint8_t *cooldown;
    uint8_t cooldown_size;
    uint8_t pwm_min;
};

enum FanMode {
    // EC control
    FAN_MODE_AUTO = 0,
    // Host control via target PWM
    FAN_MODE_PWM = 1,
    // Host control via target RPM
    FAN_MODE_RPM = 2,
};

extern bool fan_max;

extern uint8_t fan1_pwm_actual;
extern uint8_t fan1_pwm_target;
extern uint16_t fan1_rpm;
extern uint8_t fan2_pwm_actual;
extern uint8_t fan2_pwm_target;
extern uint16_t fan2_rpm;

void fan_reset(void);
#if CONFIG_FAN_CTRL_STEP
void fan_update_duty(void);
void fan_update_target(void);
#elif CONFIG_FAN_CTRL_INTERP
void fan_event(void);
#endif

enum FanMode fan_get_mode(void);
void fan_set_mode(enum FanMode);

#endif // _APP_FAN_H
