// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <common/macro.h>

static const struct FanLevel __code fan1_table[] = {
    { 50, 0, PWM_DUTY(40) },
    { 55, 48, PWM_DUTY(40) },
    { 60, 52, PWM_DUTY(55) },
    { 69, 58, PWM_DUTY(55) },
    { 74, 65, PWM_DUTY(68) },
    { 79, 72, PWM_DUTY(68) },
    { 80, 78, PWM_DUTY(72) },
    { 87, 85, PWM_DUTY(100) },
};

const struct Fan __code FAN1 = {
    .levels = fan1_table,
    .levels_size = ARRAY_SIZE(fan1_table),
};

static const struct FanLevel __code fan2_table[] = {
    { 50, 0, PWM_DUTY(0) },
    { 55, 48, PWM_DUTY(40) },
    { 62, 52, PWM_DUTY(40) },
    { 72, 66, PWM_DUTY(100) },
};

const struct Fan __code FAN2 = {
    .levels = fan2_table,
    .levels_size = ARRAY_SIZE(fan2_table),
};
