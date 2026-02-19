// SPDX-License-Identifier: GPL-3.0-only

#include <app/fan.h>
#include <app/power.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/pwm.h>
#include <drivers/dgpu/dgpu.h>

#if CONFIG_PLATFORM_INTEL
#include <app/peci.h>
#endif

bool fan_max = false;
static enum FanMode fan_mode = FAN_MODE_AUTO;

static uint8_t last_fan1_duty = 0;
static uint8_t last_fan2_duty = 0;

uint8_t fan1_pwm_actual = 0;
uint8_t fan1_pwm_target = 0;
uint16_t fan1_rpm = 0;

uint8_t fan2_pwm_actual = 0;
uint8_t fan2_pwm_target = 0;
uint16_t fan2_rpm = 0;

#define TACH_FREQ (CONFIG_CLOCK_FREQ_KHZ * 1000UL)

// Fan Speed (RPM) = 60 / (1/fs sec * {FnTMRR, FnRLRR} * P)
// - n: 1 or 2
// - P: the numbers of square pulses per revolution
// - fs: sample rate (FreqEC / 128)
// - {FnTMRR, FnTLRR} = 0000h: Fan Speed is zero
#define TACH_TO_RPM(x) (60UL * TACH_FREQ / 128UL / 2UL / (x))

#define MAX_FAN_SPEED CTR0
#define MIN_FAN_SPEED 0

#ifndef SMOOTH_FANS_UP
#define SMOOTH_FANS_UP 45 // default to ~11 seconds for full ramp-up
#endif
#ifndef SMOOTH_FANS_DOWN
#define SMOOTH_FANS_DOWN 100 // default to ~25 seconds for full ramp-down
#endif

#ifndef SMOOTH_FANS_MIN
#define SMOOTH_FANS_MIN 0 // default to smoothing all fan speed changes
#endif

#define MAX_JUMP_UP ((MAX_FAN_SPEED - MIN_FAN_SPEED) / (uint8_t)SMOOTH_FANS_UP)
#define MAX_JUMP_DOWN ((MAX_FAN_SPEED - MIN_FAN_SPEED) / (uint8_t)SMOOTH_FANS_DOWN)

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
                return 0;
            } else {
                const struct FanPoint *prev = &fan->points[i - 1];

                // If in between current temp and previous temp, interpolate
                if (temp > prev->temp) {
                    int16_t dtemp = (cur->temp - prev->temp);
                    int16_t dduty = ((int16_t)cur->duty) - ((int16_t)prev->duty);
                    return (uint8_t)(
                        ((int16_t)prev->duty) +
                        ((temp - prev->temp) * dduty) / dtemp
                    );
                }
            }
        }
    }

    // If no point is found, return 100%
    return CTR0;
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
            duty = CTR0;
        } else {
            duty = fan_heatup(fan, duty);
            duty = fan_cooldown(fan, duty);
        }
    } else {
        duty = 0;
    }

    return duty;
}

static uint16_t fan_get_tach0_rpm(void) {
    uint16_t rpm = (F1TMRR << 8) | F1TLRR;

    if (rpm)
        rpm = TACH_TO_RPM(rpm);

    return rpm;
}

static uint16_t fan_get_tach1_rpm(void) {
    uint16_t rpm = (F2TMRR << 8) | F2TLRR;

    if (rpm)
        rpm = TACH_TO_RPM(rpm);

    return rpm;
}

void fan_event(void) {
#if CONFIG_HAVE_DGPU
    int16_t sys_temp = MAX(peci_temp, dgpu_temp);
#else
    int16_t sys_temp = peci_temp;
#endif

    // set FAN1 duty
    fan1_pwm_target = fan_get_duty(&FAN1, sys_temp);
    if (fan1_pwm_target != fan1_pwm_actual) {
        TRACE("FAN1 target duty=%d\n", fan1_pwm_target);
        last_fan1_duty = fan1_pwm_target = fan_smooth(last_fan1_duty, fan1_pwm_target);
        fan1_pwm_actual = fan_max ? MAX_FAN_SPEED : fan1_pwm_target;
        TRACE("FAN1 duty smoothed=%d\n", fan1_pwm_target);
    }
    FAN1_PWM = fan1_pwm_actual;
    fan1_rpm = fan_get_tach0_rpm();

#ifdef FAN2_PWM
    // set FAN2 duty
    fan2_pwm_target = fan_get_duty(&FAN2, sys_temp);
    if (fan2_pwm_target != fan2_pwm_actual) {
        TRACE("FAN2 target duty = %d\n", fan2_pwm_target);
        last_fan2_duty = fan2_pwm_target = fan_smooth(last_fan2_duty, fan2_pwm_target);
        fan2_pwm_actual = fan_max ? MAX_FAN_SPEED : fan2_pwm_target;
        TRACE("FAN2 duty smoothed = %d\n", fan2_pwm_target);
    }
    FAN2_PWM = fan2_pwm_actual;
    fan2_rpm = fan_get_tach1_rpm();
#endif
}

enum FanMode fan_get_mode(void) {
    return fan_mode;
}

void fan_set_mode(enum FanMode mode) {
    fan_mode = mode;
}
