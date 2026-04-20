// SPDX-License-Identifier: GPL-3.0-only

#include <app/fan.h>
#include <ec/pwm.h>

bool fan_max = false;
static enum FanMode fan_mode = FAN_MODE_AUTO;

#define TACH_FREQ (CONFIG_CLOCK_FREQ_KHZ * 1000UL)

// Fan Speed (RPM) = 60 / (1/fs sec * {FnTMRR, FnRLRR} * P)
// - n: 1 or 2
// - P: the numbers of square pulses per revolution
// - fs: sample rate (FreqEC / 128)
// - {FnTMRR, FnTLRR} = 0000h: Fan Speed is zero
#define TACH_TO_RPM(x) (60UL * TACH_FREQ / 128UL / 2UL / (x))

uint16_t fan_get_tach0_rpm(void) {
    uint16_t rpm = (F1TMRR << 8) | F1TLRR;

    if (rpm)
        rpm = TACH_TO_RPM(rpm);

    return rpm;
}

uint16_t fan_get_tach1_rpm(void) {
    uint16_t rpm = (F2TMRR << 8) | F2TLRR;

    if (rpm)
        rpm = TACH_TO_RPM(rpm);

    return rpm;
}

void fan_reset(void) {
    // Do not manually set fans to maximum speed
    fan_max = false;
}

enum FanMode fan_get_mode(void) {
    return fan_mode;
}

void fan_set_mode(enum FanMode mode) {
    fan_mode = mode;
}
