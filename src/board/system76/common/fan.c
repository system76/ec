// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <board/dgpu.h>
#include <board/power.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/pwm.h>

#if CONFIG_PLATFORM_INTEL
#include <board/peci.h>
#endif

bool fan_max = false;
static enum FanMode fan_mode = FAN_MODE_AUTO;

uint8_t fan1_pwm_actual = 0;
uint8_t fan1_pwm_target = 0;
uint16_t fan1_rpm = 0;
static uint8_t fan1_level = 0;

uint8_t fan2_pwm_actual = 0;
uint8_t fan2_pwm_target = 0;
uint16_t fan2_rpm = 0;
static uint8_t fan2_level = 0;

#define TACH_FREQ (CONFIG_CLOCK_FREQ_KHZ * 1000UL)

// Fan Speed (RPM) = 60 / (1/fs sec * {FnTMRR, FnRLRR} * P)
// - n: 1 or 2
// - P: the numbers of square pulses per revolution
// - fs: sample rate (FreqEC / 128)
// - {FnTMRR, FnTLRR} = 0000h: Fan Speed is zero
#define TACH_TO_RPM(x) (60UL * TACH_FREQ / 128UL / 2UL / (x))

#define FAN_POINT(T, D) { .temp = (int16_t)(T), .duty = PWM_DUTY(D) }

// Fan cooling hysteresis in degrees Celcius
#define FAN_TEMP_HYSTERESIS 5

#ifndef FAN1_PWM_MIN
#define FAN1_PWM_MIN 0
#endif

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
    .pwm_min = PWM_DUTY(FAN1_PWM_MIN),
};

#ifdef FAN2_PWM

#ifndef FAN2_PWM_MIN
#define FAN2_PWM_MIN 0
#endif

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
    .pwm_min = PWM_DUTY(FAN2_PWM_MIN),
};

#endif // FAN2_PWM

void fan_reset(void) {
    // Do not manually set fans to maximum speed
    fan_max = false;
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

// Update the target duty of the fans based on system temps.
// Interval: 1sec
void fan_update_target(void) {
#if CONFIG_PLATFORM_INTEL
#if CONFIG_HAVE_DGPU
    int16_t sys_temp = MAX(peci_temp, dgpu_temp);
#else
    int16_t sys_temp = peci_temp;
#endif
#elif CONFIG_PLATFORM_AMD
    // TODO: AMD SB-TSI temp
    int16_t sys_temp = 50;
#endif
    uint8_t i = 0;

    // Set FAN1 target duty.
    if (fan_max) {
        fan1_pwm_target = CTR0;
    } else if (power_state != POWER_STATE_S0) {
        fan1_pwm_target = 0;
    } else {
        // Check highest level
        for (i = 0; i < FAN1.points_size; i++) {
            if (sys_temp < FAN1.points[i].temp) {
                break;
            }
        }
        if (fan1_level < i)
            fan1_level = i;

        // Check lowest level with hysteresis
        for (i = 0; i < FAN1.points_size; i++) {
            if (sys_temp < FAN1.points[i].temp - FAN_TEMP_HYSTERESIS) {
                break;
            }
        }
        if (fan1_level > i)
            fan1_level = i;

        if ((fan1_level == 0) && (sys_temp < (FAN1.points[0].temp - FAN_TEMP_HYSTERESIS))) {
            // If temp is below point 0 with hysteresis, then turn off.
            fan1_pwm_target = 0;
        } else {
            fan1_pwm_target = FAN1.points[fan1_level].duty;
        }
    }

#ifdef FAN2_PWM
    // Set FAN2 target duty.
    if (fan_max) {
        fan2_pwm_target = CTR0;
    } else if (power_state != POWER_STATE_S0) {
        fan2_pwm_target = 0;
    } else {
        // Check highest level
        for (i = 0; i < FAN2.points_size; i++) {
            if (sys_temp < FAN2.points[i].temp) {
                break;
            }
        }
        if (fan2_level < i)
            fan2_level = i;

        // Check lowest level with hysteresis
        for (i = 0; i < FAN2.points_size; i++) {
            if (sys_temp < FAN2.points[i].temp - FAN_TEMP_HYSTERESIS) {
                break;
            }
        }
        if (fan2_level > i)
            fan2_level = i;

        if ((fan2_level == 0) && (sys_temp < (FAN2.points[0].temp - FAN_TEMP_HYSTERESIS))) {
            // If temp is below point 0 with hysteresis, then turn off.
            fan2_pwm_target = 0;
        } else {
            fan2_pwm_target = FAN2.points[fan2_level].duty;
        }
    }
#endif
}

// Update the actual duty of the fans to move towards the target duty.
// The duty is changed by 1 every interval to give a smoothing effect and to
// avoid large, sudden changes.
// Enabling fan max toggle and exiting S0 will cause duty to immediately
// change instead of stepping to provide the desired effects.
// Interval: 100ms
void fan_update_duty(void) {
    // Move FAN1 duty towards target.
    if (fan_max) {
        fan1_pwm_actual = CTR0;
    } else if (power_state != POWER_STATE_S0) {
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
    // Update RPM value for reporting to ACPI.
    fan1_rpm = fan_get_tach0_rpm();

#ifdef FAN2_PWM
    // Move FAN2 duty towards target.
    if (fan_max) {
        fan2_pwm_actual = CTR0;
    } else if (power_state != POWER_STATE_S0) {
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
    // Update RPM value for reporting to ACPI.
    fan2_rpm = fan_get_tach1_rpm();
#endif
}

enum FanMode fan_get_mode(void) {
    return fan_mode;
}

void fan_set_mode(enum FanMode mode) {
    fan_mode = mode;
}
