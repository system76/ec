// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <common/macro.h>

static const struct FanPoint __code FAN1_POINTS[] = {
    FAN_POINT(55, 25),
    FAN_POINT(65, 30),
    FAN_POINT(70, 40),
    FAN_POINT(75, 60),
    FAN_POINT(80, 75),
    FAN_POINT(85, 90),
    FAN_POINT(90, 100),
};

const struct Fan __code FAN1 = {
    .points = FAN1_POINTS,
    .points_size = ARRAY_SIZE(FAN1_POINTS),
    .pwm_min = PWM_DUTY(25),
};

static const struct FanPoint __code FAN2_POINTS[] = {
    FAN_POINT(55, 25),
    FAN_POINT(65, 30),
    FAN_POINT(70, 40),
    FAN_POINT(75, 60),
    FAN_POINT(80, 75),
    FAN_POINT(85, 90),
    FAN_POINT(90, 100),
};

const struct Fan __code FAN2 = {
    .points = FAN2_POINTS,
    .points_size = ARRAY_SIZE(FAN2_POINTS),
    .pwm_min = PWM_DUTY(25),
};
