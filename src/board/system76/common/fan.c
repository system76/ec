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

    // Set FAN1 duty
    fan1_info.pwm_target = fan_duty(&FAN1, sys_temp);
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
        } else if (fan1_info.pwm_actual > fan1_info.pwm_target) {
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
    // set FAN2 duty
    fan2_info.pwm_target = fan_duty(&FAN2, sys_temp);
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
        } else if (fan2_info.pwm_actual > fan2_info.pwm_target) {
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
