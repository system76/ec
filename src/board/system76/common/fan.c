// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <board/dgpu.h>
#include <board/peci.h>
#include <board/power.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/pwm.h>

bool fan_max = false;

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

#define FAN_POINT(T, D) { .temp = (int16_t)(T), .duty = PWM_DUTY(D) }

#ifndef FAN1_PWM_MIN
#define FAN1_PWM_MIN 0
#endif

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
    .pwm_min = PWM_DUTY(FAN1_PWM_MIN),
};

#ifdef FAN2_PWM

#ifndef FAN2_PWM_MIN
#define FAN2_PWM_MIN 0
#endif

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
    .pwm_min = PWM_DUTY(FAN2_PWM_MIN),
};

#endif // FAN2_PWM

void fan_reset(void) {
    // Do not manually set fans to maximum speed
    fan_max = false;
}

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
                return prev->duty;
            }
        }
    }

    // If no point is found, return 100%
    return CTR0;
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

    duty = fan_duty(fan, temp);
    duty = fan_heatup(fan, duty);
    duty = fan_cooldown(fan, duty);

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

    // Fan update interval is 100ms (main.c). The event changes PWM duty
    // by 1 every interval to give a smoothing effect.

    // Enabling fan max toggle and exiting S0 will cause duty to immediately
    // change instead of stepping to provide the desired effects.

    // Set FAN1 duty
    fan1_pwm_target = fan_get_duty(&FAN1, sys_temp);
    if (fan_max) {
        fan1_pwm_target = CTR0;
        fan1_pwm_actual = CTR0;
    } else if (power_state != POWER_STATE_S0) {
        fan1_pwm_target = 0;
        fan1_pwm_actual = 0;
    } else {
        if (fan1_pwm_actual < fan1_pwm_target) {
            if (fan1_pwm_actual < CTR0) {
                fan1_pwm_actual++;
                if (fan1_pwm_actual < FAN1.pwm_min) {
                    fan1_pwm_actual = FAN1.pwm_min;
                }
            }
        } else if (fan1_pwm_actual > fan1_pwm_target) {
            if (fan1_pwm_actual > 0) {
                fan1_pwm_actual--;
                if (fan1_pwm_actual < FAN1.pwm_min) {
                    fan1_pwm_actual = 0;
                }
            }
        }
    }
    TRACE("FAN1 duty=%d\n", fan1_pwm_actual);
    FAN1_PWM = fan1_pwm_actual;
    fan1_rpm = fan_get_tach0_rpm();

#ifdef FAN2_PWM
    // set FAN2 duty
    fan2_pwm_target = fan_get_duty(&FAN2, sys_temp);
    if (fan_max) {
        fan2_pwm_target = CTR0;
        fan2_pwm_actual = CTR0;
    } else if (power_state != POWER_STATE_S0) {
        fan2_pwm_target = 0;
        fan2_pwm_actual = 0;
    } else {
        if (fan2_pwm_actual < fan2_pwm_target) {
            if (fan2_pwm_actual < CTR0) {
                fan2_pwm_actual++;
                if (fan2_pwm_actual < FAN2.pwm_min) {
                    fan2_pwm_actual = FAN2.pwm_min;
                }
            }
        } else if (fan2_pwm_actual > fan2_pwm_target) {
            if (fan2_pwm_actual > 0) {
                fan2_pwm_actual--;
                if (fan2_pwm_actual < FAN2.pwm_min) {
                    fan2_pwm_actual = 0;
                }
            }
        }
    }
    TRACE("FAN2 duty=%d\n", fan2_pwm_actual);
    FAN2_PWM = fan2_pwm_actual;
    fan2_rpm = fan_get_tach1_rpm();
#endif
}
