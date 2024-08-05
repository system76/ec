// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <common/macro.h>

static const struct FanPoint __code FAN1_POINTS[] = {
    FAN_POINT(48, 27),
    FAN_POINT(52, 27),
    FAN_POINT(56, 33),
    FAN_POINT(60, 37),
    FAN_POINT(75, 53),
    FAN_POINT(83, 65),
    FAN_POINT(87, 75),
    FAN_POINT(89, 77),
};

const struct Fan __code FAN1 = {
    .points = FAN1_POINTS,
    .points_size = ARRAY_SIZE(FAN1_POINTS),
    .pwm_min = PWM_DUTY(27),
};

static const struct FanPoint __code FAN2_POINTS[] = {
    FAN_POINT(48, 27),
    FAN_POINT(52, 27),
    FAN_POINT(56, 33),
    FAN_POINT(60, 37),
    FAN_POINT(75, 53),
    FAN_POINT(83, 65),
    FAN_POINT(87, 75),
    FAN_POINT(89, 77),
};

const struct Fan __code FAN2 = {
    .points = FAN2_POINTS,
    .points_size = ARRAY_SIZE(FAN2_POINTS),
    .pwm_min = PWM_DUTY(27),
};
