// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <common/macro.h>

static const struct FanPoint __code FAN1_POINTS[] = {
    FAN_POINT(50, 40),
    FAN_POINT(55, 40),
    FAN_POINT(60, 55),
    FAN_POINT(69, 55),
    FAN_POINT(74, 68),
    FAN_POINT(79, 68),
    FAN_POINT(80, 72),
    FAN_POINT(87, 100),
};

const struct Fan __code FAN1 = {
    .points = FAN1_POINTS,
    .points_size = ARRAY_SIZE(FAN1_POINTS),
};

static const struct FanPoint __code FAN2_POINTS[] = {
    FAN_POINT(50, 40),
    FAN_POINT(56, 40),
    FAN_POINT(72, 100),
};

const struct Fan __code FAN2 = {
    .points = FAN2_POINTS,
    .points_size = ARRAY_SIZE(FAN2_POINTS),
};
