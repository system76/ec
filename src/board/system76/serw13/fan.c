// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <common/macro.h>

static const struct FanPoint __code FAN1_POINTS[] = {
    FAN_POINT(50, 28),
    FAN_POINT(55, 28),
    FAN_POINT(60, 40),
    FAN_POINT(68, 40),
    FAN_POINT(73, 50),
    FAN_POINT(87, 100),
};

const struct Fan __code FAN1 = {
    .points = FAN1_POINTS,
    .points_size = ARRAY_SIZE(FAN1_POINTS),
    .pwm_min = PWM_DUTY(28),
};

static const struct FanPoint __code FAN2_POINTS[] = {
    FAN_POINT(50, 28),
    FAN_POINT(55, 28),
    FAN_POINT(60, 40),
    FAN_POINT(68, 40),
    FAN_POINT(73, 50),
    FAN_POINT(87, 100),
};

const struct Fan __code FAN2 = {
    .points = FAN2_POINTS,
    .points_size = ARRAY_SIZE(FAN2_POINTS),
    .pwm_min = PWM_DUTY(28),
};
