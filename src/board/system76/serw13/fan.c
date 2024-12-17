// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <common/macro.h>

static const struct FanLevel __code fan1_table[] = {
    { 50, 0, PWM_DUTY(0) },
    { 55, 48, PWM_DUTY(28) },
    { 60, 56, PWM_DUTY(40) },
    { 68, 65, PWM_DUTY(40) },
    { 73, 70, PWM_DUTY(50) },
    { 87, 85, PWM_DUTY(100) },
};

const struct Fan __code FAN1 = {
    .levels = fan1_table,
    .levels_size = ARRAY_SIZE(fan1_table),
    .pwm_min = PWM_DUTY(28),
};

static const struct FanLevel __code fan2_table[] = {
    { 50, 0, PWM_DUTY(0) },
    { 55, 48, PWM_DUTY(28) },
    { 60, 56, PWM_DUTY(40) },
    { 68, 65, PWM_DUTY(40) },
    { 73, 70, PWM_DUTY(50) },
    { 87, 85, PWM_DUTY(100) },
};

const struct Fan __code FAN2 = {
    .levels = fan2_table,
    .levels_size = ARRAY_SIZE(fan2_table),
    .pwm_min = PWM_DUTY(28),
};
