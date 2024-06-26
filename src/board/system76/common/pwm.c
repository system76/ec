// SPDX-License-Identifier: GPL-3.0-only

#include <board/pwm.h>
#include <common/macro.h>

#define TACH_FREQ (CONFIG_CLOCK_FREQ_KHZ * 1000UL)

// Fan Speed (RPM) = 60 / (1/fs sec * {FnTMRR, FnRLRR} * P)
// - n: 1 or 2
// - P: the numbers of square pulses per revolution
// - fs: sample rate (FreqEC / 128)
// - {FnTMRR, FnTLRR} = 0000h: Fan Speed is zero
#define TACH_TO_RPM(x) (60UL * TACH_FREQ / 128UL / 2UL / (x))

int16_t pwm_tach0_rpm = -1;
int16_t pwm_tach1_rpm = -1;

void pwm_init(void) {
    // Set T0CHSEL to TACH0A and T1CHSEL to TACH1A
    TSWCTLR = 0;

    // Disable PWM
    ZTIER = 0;

    // Set prescalar clock frequency to EC clock
    PCFSR = 0b01;

    // Use C0CPRS and CTR0 for all channels
    PCSSGL = 0;
    PCSSGH = 0;

    // Set clock prescaler to 0 + 1
    C0CPRS = 0;

    // Set cycle time to 255 + 1
    CTR0 = 255;

    // Turn off CPU fan (temperature control in peci_get_fan_duty)
    DCR2 = 0;
#if HAVE_CPU_FAN2
    DCR3 = 0;
#endif

#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
    // Reload counters when they reach 0 instead of immediately
    PWMLCCR = 0xFF;
#endif

    // Enable PWM
    ZTIER = BIT(1);
}

int16_t pwm_get_tach0_rpm(void) {
    uint16_t rpm = (F1TMRR << 8) | F1TLRR;

    if (rpm)
        rpm = TACH_TO_RPM(rpm);

    return rpm;
}

int16_t pwm_get_tach1_rpm(void) {
    uint16_t rpm = (F2TMRR << 8) | F2TLRR;

    if (rpm)
        rpm = TACH_TO_RPM(rpm);

    return rpm;
}
