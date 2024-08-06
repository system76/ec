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

static uint8_t fan1_level = 0;
#ifdef FAN2_PWM
static uint8_t fan2_level = 0;
#endif

struct FanInfo fan1_info = {
    .pwm_actual = 0,
    .pwm_target = 0,
    .rpm = 0,
};

#ifdef FAN2_PWM
struct FanInfo fan2_info = {
    .pwm_actual = 0,
    .pwm_target = 0,
    .rpm = 0,
};
#endif

#define TACH_FREQ (CONFIG_CLOCK_FREQ_KHZ * 1000UL)

// Fan Speed (RPM) = 60 / (1/fs sec * {FnTMRR, FnRLRR} * P)
// - n: 1 or 2
// - P: the numbers of square pulses per revolution
// - fs: sample rate (FreqEC / 128)
// - {FnTMRR, FnTLRR} = 0000h: Fan Speed is zero
#define TACH_TO_RPM(x) (60UL * TACH_FREQ / 128UL / 2UL / (x))

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

    // Fan update interval is 100ms (main.c). The event changes PWM duty
    // by 1 every interval to give a smoothing effect.

    // Enabling fan max toggle and exiting S0 will cause duty to immediately
    // change instead of stepping to provide the desired effects.

    // Get FAN1 target duty
    if (fan1_level < FAN1.levels_size) {
        if (sys_temp >= FAN1.levels[fan1_level].temp_up) {
            fan1_level++;
        }
    }
    if (fan1_level > 0) {
        if (sys_temp <= FAN1.levels[fan1_level].temp_down) {
            fan1_level--;
        }
    }

    fan1_info.pwm_target = FAN1.levels[fan1_level].duty;

    // Set FAN1 duty
    if (fan_max) {
        fan1_info.pwm_target = CTR0;
        fan1_info.pwm_actual = CTR0;
    } else if (power_state != POWER_STATE_S0) {
        fan1_info.pwm_target = 0;
        fan1_info.pwm_actual = 0;
    } else {
        if (fan1_info.pwm_actual < fan1_info.pwm_target) {
            if (fan1_info.pwm_actual < CTR0) {
                fan1_info.pwm_actual++;
                if (fan1_info.pwm_actual < FAN1.pwm_min) {
                    fan1_info.pwm_actual = FAN1.pwm_min;
                }
            }
        }
        if (fan1_info.pwm_actual > fan1_info.pwm_target) {
            if (fan1_info.pwm_actual > 0) {
                fan1_info.pwm_actual--;
                if (fan1_info.pwm_actual < FAN1.pwm_min) {
                    fan1_info.pwm_actual = 0;
                }
            }
        }
    }
    TRACE("FAN1 duty=%d\n", fan1_info.pwm_actual);
    FAN1_PWM = fan1_info.pwm_actual;
    fan1_info.rpm = fan_get_tach0_rpm();

#ifdef FAN2_PWM
    // Get FAN2 target duty
    if (fan2_level < FAN2.levels_size) {
        if (sys_temp >= FAN2.levels[fan2_level].temp_up) {
            fan2_level++;
        }
    }
    if (fan2_level > 0) {
        if (sys_temp <= FAN2.levels[fan2_level].temp_down) {
            fan2_level--;
        }
    }

    fan2_info.pwm_target = FAN2.levels[fan2_level].duty;

    // set FAN2 duty
    if (fan_max) {
        fan2_info.pwm_target = CTR0;
        fan2_info.pwm_actual = CTR0;
    } else if (power_state != POWER_STATE_S0) {
        fan2_info.pwm_target = 0;
        fan2_info.pwm_actual = 0;
    } else {
        if (fan2_info.pwm_actual < fan2_info.pwm_target) {
            if (fan2_info.pwm_actual < CTR0) {
                fan2_info.pwm_actual++;
                if (fan2_info.pwm_actual < FAN2.pwm_min) {
                    fan2_info.pwm_actual = FAN2.pwm_min;
                }
            }
        }
        if (fan2_info.pwm_actual > fan2_info.pwm_target) {
            if (fan2_info.pwm_actual > 0) {
                fan2_info.pwm_actual--;
                if (fan2_info.pwm_actual < FAN2.pwm_min) {
                    fan2_info.pwm_actual = 0;
                }
            }
        }
    }
    TRACE("FAN2 duty=%d\n", fan2_info.pwm_actual);
    FAN2_PWM = fan2_info.pwm_actual;
    fan2_info.rpm = fan_get_tach1_rpm();
#endif
}
