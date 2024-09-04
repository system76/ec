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

static uint8_t fan1_level = 0;
static uint8_t fan2_level = 0;

#define FAN_LEVEL(up, down, target) \
    { \
        .temp_up = (up), \
        .temp_down = (down), \
        .duty = PWM_DUTY(target), \
    }

#define TACH_FREQ (CONFIG_CLOCK_FREQ_KHZ * 1000UL)

// Fan Speed (RPM) = 60 / (1/fs sec * {FnTMRR, FnRLRR} * P)
// - n: 1 or 2
// - P: the numbers of square pulses per revolution
// - fs: sample rate (FreqEC / 128)
// - {FnTMRR, FnTLRR} = 0000h: Fan Speed is zero
#define TACH_TO_RPM(x) (60UL * TACH_FREQ / 128UL / 2UL / (x))

#ifndef FAN1_PWM_MIN
#define FAN1_PWM_MIN 0
#endif

// Fan curve with temperature in degrees C, duty cycle in percent
static const struct FanLevel __code FAN1_LEVELS[] = {
#ifndef FAN1_TABLE
#error Board must declare fan table
#else
    FAN1_TABLE
#endif
};

static const struct Fan __code FAN1 = {
    .levels = FAN1_LEVELS,
    .levels_size = ARRAY_SIZE(FAN1_LEVELS),
    .pwm_min = PWM_DUTY(FAN1_PWM_MIN),
};

#ifdef FAN2_PWM

#ifndef FAN2_PWM_MIN
#define FAN2_PWM_MIN 0
#endif

// Fan curve with temperature in degrees C, duty cycle in percent
static const struct FanLevel __code FAN2_LEVELS[] = {
#ifndef FAN2_TABLE
#error Board must declare fan table
#else
    FAN2_TABLE
#endif
};

static const struct Fan __code FAN2 = {
    .levels = FAN2_LEVELS,
    .levels_size = ARRAY_SIZE(FAN2_LEVELS),
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

    // Get FAN1 target duty
    if (sys_temp >= FAN1.levels[fan1_level].temp_up) {
        if (fan1_level < FAN1.levels_size) {
            fan1_level++;
        }
    }
    if (sys_temp <= FAN1.levels[fan1_level].temp_down) {
        if (fan1_level > 0) {
            fan1_level--;
        }
    }
    fan1_pwm_target = FAN1.levels[fan1_level].duty;

    // Set FAN1 duty
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
    // Get FAN2 target duty
    if (sys_temp >= FAN2.levels[fan2_level].temp_up) {
        if (fan2_level < FAN2.levels_size) {
            fan2_level++;
        }
    }
    if (sys_temp <= FAN2.levels[fan2_level].temp_down) {
        if (fan2_level > 0) {
            fan2_level--;
        }
    }
    fan2_pwm_target = FAN2.levels[fan2_level].duty;

    // Set FAN2 duty
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
