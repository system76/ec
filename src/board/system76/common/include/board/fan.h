// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_FAN_H
#define _BOARD_FAN_H

#include <stdbool.h>
#include <stdint.h>

#define PWM_DUTY(X) ((uint8_t)(((((uint16_t)(X)) * 255) + 99) / 100))
#define MAX_FAN_SPEED PWM_DUTY(100)
#define MIN_FAN_SPEED PWM_DUTY(0)

#ifndef SMOOTH_FANS
  #define SMOOTH_FANS 40
#endif

#ifndef SYNC_FANS
  #define SYNC_FANS 1
#endif

#if SMOOTH_FANS != 0
  #ifndef SMOOTH_FANS_UP
    #define SMOOTH_FANS_UP SMOOTH_FANS
  #endif
  #ifndef SMOOTH_FANS_DOWN
    #define SMOOTH_FANS_DOWN SMOOTH_FANS
  #endif
#endif

struct FanPoint {
    int16_t temp;
    uint8_t duty;
};

struct Fan {
    const struct FanPoint * points;
    uint8_t points_size;
    uint8_t * heatup;
    uint8_t heatup_size;
    uint8_t * cooldown;
    uint8_t cooldown_size;
    bool interpolate;
};

extern bool fan_max;

void fan_reset(void);

uint8_t fan_duty(const struct Fan * fan, int16_t temp) __reentrant;
void fan_duty_set(uint8_t peci_fan_duty, uint8_t dgpu_fan_duty) __reentrant;
uint8_t fan_heatup(const struct Fan * fan, uint8_t duty) __reentrant;
uint8_t fan_cooldown(const struct Fan * fan, uint8_t duty) __reentrant;
uint8_t fan_smooth(uint8_t last_duty, uint8_t duty) __reentrant;

#endif // _BOARD_FAN_H
