// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <common/macro.h>

static const struct FanLevel __code fan1_table[] = {
    { 50, 0, PWM_DUTY(0) },
    { 58, 48, PWM_DUTY(28) },
    { 60, 55, PWM_DUTY(40) },
    { 65, 60, PWM_DUTY(60) },
    { 70, 65, PWM_DUTY(75) },
    { 75, 70, PWM_DUTY(90) },
    { 80, 75, PWM_DUTY(100) },
};

const struct Fan __code FAN1 = {
    .levels = fan1_table,
    .levels_size = ARRAY_SIZE(fan1_table),
    .pwm_min = PWM_DUTY(20),
};

static const struct FanLevel __code fan2_table[] = {
    { 50, 0, PWM_DUTY(0) },
    { 58, 48, PWM_DUTY(28) },
    { 60, 55, PWM_DUTY(40) },
    { 65, 60, PWM_DUTY(60) },
    { 70, 65, PWM_DUTY(75) },
    { 75, 70, PWM_DUTY(90) },
    { 80, 75, PWM_DUTY(100) },
};

const struct Fan __code FAN2 = {
    .levels = fan2_table,
    .levels_size = ARRAY_SIZE(fan2_table),
    .pwm_min = PWM_DUTY(20),
};
