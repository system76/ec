// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <common/macro.h>

static const struct FanLevel __code fan1_table[] = {
    { 70, 0, PWM_DUTY(0) },
    { 70, 65, PWM_DUTY(40) },
    { 75, 70, PWM_DUTY(50) },
    { 80, 75, PWM_DUTY(60) },
    { 85, 80, PWM_DUTY(65) },
    { 90, 85, PWM_DUTY(65) },
};

const struct Fan __code FAN1 = {
    .levels = fan1_table,
    .levels_size = ARRAY_SIZE(fan1_table),
};
