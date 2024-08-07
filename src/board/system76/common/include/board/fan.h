// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_FAN_H
#define _BOARD_FAN_H

#include <stdbool.h>
#include <stdint.h>

#define PWM_DUTY(X) ((uint8_t)(((((uint16_t)(X)) * 255) + 99) / 100))
#define MAX_FAN_SPEED PWM_DUTY(100)
#define MIN_FAN_SPEED PWM_DUTY(0)

#ifndef SMOOTH_FANS
#define SMOOTH_FANS 1 // default to fan smoothing
#endif

#ifndef SYNC_FANS
#define SYNC_FANS 1 // default to syncing fan speeds
#endif

#if SMOOTH_FANS != 0
#ifndef SMOOTH_FANS_UP
#define SMOOTH_FANS_UP 45 // default to ~11 seconds for full ramp-up
#endif
#ifndef SMOOTH_FANS_DOWN
#define SMOOTH_FANS_DOWN 100 // default to ~25 seconds for full ramp-down
#endif
#endif

#ifndef SMOOTH_FANS_MIN
#define SMOOTH_FANS_MIN 0 // default to smoothing all fan speed changes
#endif

struct FanPoint {
    int16_t temp;
    uint8_t duty;
};

struct Fan {
    const struct FanPoint *points;
    uint8_t points_size;
    uint8_t *heatup;
    uint8_t heatup_size;
    uint8_t *cooldown;
    uint8_t cooldown_size;
    bool interpolate;
};

extern bool fan_max;

void fan_reset(void);
void fan_update_duty(void);

#endif // _BOARD_FAN_H
