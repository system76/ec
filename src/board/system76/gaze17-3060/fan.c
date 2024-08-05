// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <common/macro.h>

static const struct FanPoint __code FAN1_POINTS[] = {
    FAN_POINT(60, 40),
    FAN_POINT(65, 60),
    FAN_POINT(70, 75),
    FAN_POINT(75, 90),
    FAN_POINT(80, 100),
};

const struct Fan __code FAN1 = {
    .points = FAN1_POINTS,
    .points_size = ARRAY_SIZE(FAN1_POINTS),
};

static const struct FanPoint __code FAN2_POINTS[] = {
    FAN_POINT(60, 40),
    FAN_POINT(65, 60),
    FAN_POINT(70, 75),
    FAN_POINT(75, 90),
    FAN_POINT(80, 100),
};

const struct Fan __code FAN2 = {
    .points = FAN2_POINTS,
    .points_size = ARRAY_SIZE(FAN2_POINTS),
};
