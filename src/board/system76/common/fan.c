// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <board/dgpu.h>
#include <board/peci.h>
#include <board/power.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/pwm.h>

bool fan_max = false;

static uint8_t last_fan1_duty = 0;
static uint8_t last_fan2_duty = 0;

#define FAN_POINT(T, D) { .temp = (int16_t)(T), .duty = PWM_DUTY(D) }

#if SMOOTH_FANS != 0
#define MAX_JUMP_UP ((MAX_FAN_SPEED - MIN_FAN_SPEED) / (uint8_t)SMOOTH_FANS_UP)
#define MAX_JUMP_DOWN ((MAX_FAN_SPEED - MIN_FAN_SPEED) / (uint8_t)SMOOTH_FANS_DOWN)
#else
#define MAX_JUMP_UP (MAX_FAN_SPEED - MIN_FAN_SPEED)
#define MAX_JUMP_DOWN (MAX_FAN_SPEED - MIN_FAN_SPEED)
#endif

#define MIN_SPEED_TO_SMOOTH PWM_DUTY(SMOOTH_FANS_MIN)

// Fan speed is the lowest requested over HEATUP seconds
#ifndef BOARD_FAN1_HEATUP
#define BOARD_FAN1_HEATUP 4
#endif

static uint8_t FAN1_HEATUP[BOARD_FAN1_HEATUP] = { 0 };

// Fan speed is the highest HEATUP speed over COOLDOWN seconds
#ifndef BOARD_FAN1_COOLDOWN
#define BOARD_FAN1_COOLDOWN 10
#endif

static uint8_t FAN1_COOLDOWN[BOARD_FAN1_COOLDOWN] = { 0 };

// Fan curve with temperature in degrees C, duty cycle in percent
static const struct FanPoint __code FAN1_POINTS[] = {
#ifndef BOARD_FAN1_POINTS
#error Board must declare fan points
#else
    BOARD_FAN1_POINTS
#endif
};

static const struct Fan __code FAN1 = {
    .points = FAN1_POINTS,
    .points_size = ARRAY_SIZE(FAN1_POINTS),
    .heatup = FAN1_HEATUP,
    .heatup_size = ARRAY_SIZE(FAN1_HEATUP),
    .cooldown = FAN1_COOLDOWN,
    .cooldown_size = ARRAY_SIZE(FAN1_COOLDOWN),
    .interpolate = SMOOTH_FANS != 0,
};

#ifdef FAN2_PWM

// Fan speed is the lowest requested over HEATUP seconds
#ifndef BOARD_FAN2_HEATUP
#define BOARD_FAN2_HEATUP 4
#endif

static uint8_t FAN2_HEATUP[BOARD_FAN2_HEATUP] = { 0 };

// Fan speed is the highest HEATUP speed over COOLDOWN seconds
#ifndef BOARD_FAN2_COOLDOWN
#define BOARD_FAN2_COOLDOWN 10
#endif

static uint8_t FAN2_COOLDOWN[BOARD_FAN2_COOLDOWN] = { 0 };

// Fan curve with temperature in degrees C, duty cycle in percent
static const struct FanPoint __code FAN2_POINTS[] = {
#ifndef BOARD_FAN2_POINTS
#error Board must declare fan points
#else
    BOARD_FAN2_POINTS
#endif
};

static const struct Fan __code FAN2 = {
    .points = FAN2_POINTS,
    .points_size = ARRAY_SIZE(FAN2_POINTS),
    .heatup = FAN2_HEATUP,
    .heatup_size = ARRAY_SIZE(FAN2_HEATUP),
    .cooldown = FAN2_COOLDOWN,
    .cooldown_size = ARRAY_SIZE(FAN2_COOLDOWN),
    .interpolate = SMOOTH_FANS != 0,
};

#endif // FAN2_PWM

void fan_reset(void) {
    // Do not manually set fans to maximum speed
    fan_max = false;
}

// Get duty cycle based on temperature, adapted from
// https://github.com/pop-os/system76-power/blob/master/src/fan.rs
static uint8_t fan_duty(const struct Fan *const fan, int16_t temp) {
    for (uint8_t i = 0; i < fan->points_size; i++) {
        const struct FanPoint *cur = &fan->points[i];

        // If exactly the current temp, return the current duty
        if (temp == cur->temp) {
            return cur->duty;
        } else if (temp < cur->temp) {
            // If lower than first temp, return 0%
            if (i == 0) {
                return MIN_FAN_SPEED;
            } else {
                const struct FanPoint *prev = &fan->points[i - 1];

                if (fan->interpolate) {
                    // If in between current temp and previous temp, interpolate
                    if (temp > prev->temp) {
                        int16_t dtemp = (cur->temp - prev->temp);
                        int16_t dduty = ((int16_t)cur->duty) - ((int16_t)prev->duty);
                        return (uint8_t)(
                            ((int16_t)prev->duty) +
                            ((temp - prev->temp) * dduty) / dtemp
                        );
                    }
                } else {
                    return prev->duty;
                }
            }
        }
    }

    // If no point is found, return 100%
    return MAX_FAN_SPEED;
}

static uint8_t fan_smooth(uint8_t last_duty, uint8_t duty) {
    uint8_t next_duty = duty;

    // ramping down
    if (duty < last_duty) {
        // out of bounds (lower) safeguard
        uint8_t smoothed = last_duty < MIN_FAN_SPEED + MAX_JUMP_DOWN
            ? MIN_FAN_SPEED
            : last_duty - MAX_JUMP_DOWN;

        // use smoothed value if above min and if smoothed is closer than raw
        if (last_duty > MIN_SPEED_TO_SMOOTH && smoothed > duty) {
            next_duty = smoothed;
        }
    }

    // ramping up
    if (duty > last_duty) {
        // out of bounds (higher) safeguard
        uint8_t smoothed = last_duty > MAX_FAN_SPEED - MAX_JUMP_UP
            ? MAX_FAN_SPEED
            : last_duty + MAX_JUMP_UP;

        // use smoothed value if above min and if smoothed is closer than raw
        if (duty > MIN_SPEED_TO_SMOOTH && smoothed < duty) {
            next_duty = smoothed;
        }
    }

    return next_duty;
}

static uint8_t fan_heatup(const struct Fan *const fan, uint8_t duty) {
    uint8_t lowest = duty;

    uint8_t i;
    for (i = 0; (i + 1) < fan->heatup_size; i++) {
        uint8_t value = fan->heatup[i + 1];
        if (value < lowest) {
            lowest = value;
        }
        fan->heatup[i] = value;
    }
    fan->heatup[i] = duty;

    return lowest;
}

static uint8_t fan_cooldown(const struct Fan *const fan, uint8_t duty) {
    uint8_t highest = duty;

    uint8_t i;
    for (i = 0; (i + 1) < fan->cooldown_size; i++) {
        uint8_t value = fan->cooldown[i + 1];
        if (value > highest) {
            highest = value;
        }
        fan->cooldown[i] = value;
    }
    fan->cooldown[i] = duty;

    return highest;
}

static uint8_t fan_get_duty(const struct Fan *const fan, int16_t temp) {
    uint8_t duty;

    if (power_state == POWER_STATE_S0) {
        duty = fan_duty(fan, temp);
        if (fan_max) {
            duty = PWM_DUTY(100);
        } else {
            duty = fan_heatup(fan, duty);
            duty = fan_cooldown(fan, duty);
        }
    } else {
        duty = PWM_DUTY(0);
    }

    return duty;
}

void fan_update_duty(void) {
#if defined(FAN2_PWM) && !CONFIG_HAVE_DGPU
    int16_t dgpu_temp = peci_temp;
#endif

    uint8_t fan1_duty = fan_get_duty(&FAN1, peci_temp);
#ifdef FAN2_PWM
    uint8_t fan2_duty = fan_get_duty(&FAN2, dgpu_temp);

#if SYNC_FANS != 0
    fan1_duty = MAX(fan1_duty, fan2_duty);
    fan2_duty = MAX(fan1_duty, fan2_duty);
#endif // SYNC_FANS
#endif // FAN2_PWM

    // set FAN1 duty
    if (fan1_duty != FAN1_PWM) {
        TRACE("FAN1 fan_duty_raw=%d\n", fan1_duty);
        last_fan1_duty = fan1_duty = fan_smooth(last_fan1_duty, fan1_duty);
        FAN1_PWM = fan_max ? MAX_FAN_SPEED : fan1_duty;
        TRACE("FAN1 fan_duty_smoothed=%d\n", fan1_duty);
    }

#ifdef FAN2_PWM
    // set FAN2 duty
    if (fan2_duty != FAN2_PWM) {
        TRACE("FAN2 fan_duty_raw=%d\n", fan2_duty);
        last_fan2_duty = fan2_duty = fan_smooth(last_fan2_duty, fan2_duty);
        FAN2_PWM = fan_max ? MAX_FAN_SPEED : fan2_duty;
        TRACE("FAN2 fan_duty_smoothed=%d\n", fan2_duty);
    }
#endif
}
