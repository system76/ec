// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>
#include <common/debug.h>
#include <ec/pwm.h>

#if SMOOTH_FANS != 0
#define MAX_JUMP_UP ((MAX_FAN_SPEED - MIN_FAN_SPEED) / (uint8_t)SMOOTH_FANS_UP)
#define MAX_JUMP_DOWN ((MAX_FAN_SPEED - MIN_FAN_SPEED) / (uint8_t)SMOOTH_FANS_DOWN)
#else
#define MAX_JUMP_UP (MAX_FAN_SPEED - MIN_FAN_SPEED)
#define MAX_JUMP_DOWN (MAX_FAN_SPEED - MIN_FAN_SPEED)
#endif

#define MIN_SPEED_TO_SMOOTH PWM_DUTY(SMOOTH_FANS_MIN)

bool fan_max = false;
uint8_t acpi_peci_fan_duty = 0;
uint8_t acpi_dgpu_fan_duty = 0;
uint8_t last_duty_dgpu = 0;
uint8_t last_duty_peci = 0;

void fan_reset(void) {
    // Do not manually set fans to maximum speed
    fan_max = false;
}

// Get duty cycle based on temperature, adapted from
// https://github.com/pop-os/system76-power/blob/master/src/fan.rs
uint8_t fan_duty(const struct Fan *fan, int16_t temp) __reentrant {
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
                        // clang-format off
                        return (uint8_t)(
                            ((int16_t)prev->duty) +
                            ((temp - prev->temp) * dduty) / dtemp
                        );
                        // clang-format on
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

void fan_duty_set(uint8_t peci_fan_duty, uint8_t dgpu_fan_duty) __reentrant {
#if SYNC_FANS != 0
    peci_fan_duty = peci_fan_duty > dgpu_fan_duty ? peci_fan_duty : dgpu_fan_duty;
    dgpu_fan_duty = peci_fan_duty > dgpu_fan_duty ? peci_fan_duty : dgpu_fan_duty;
#endif

    // allow for ACPI to request a higher duty
    peci_fan_duty = peci_fan_duty > acpi_peci_fan_duty ? peci_fan_duty : acpi_peci_fan_duty;
    dgpu_fan_duty = dgpu_fan_duty > acpi_dgpu_fan_duty ? dgpu_fan_duty : acpi_dgpu_fan_duty;

    // set PECI fan duty
    if (peci_fan_duty != DCR2) {
        TRACE("PECI acpi_fan_duty_raw=%d\n", acpi_peci_fan_duty);
        TRACE("PECI fan_duty_raw=%d\n", peci_fan_duty);
        last_duty_peci = peci_fan_duty = fan_smooth(last_duty_peci, peci_fan_duty);
        DCR2 = fan_max ? MAX_FAN_SPEED : peci_fan_duty;
        TRACE("PECI fan_duty_smoothed=%d\n", peci_fan_duty);
    }

    // set dGPU fan duty
    if (dgpu_fan_duty != DCR4) {
        TRACE("DGPU acpi_fan_duty_raw=%d\n", acpi_peci_fan_duty);
        TRACE("DGPU fan_duty_raw=%d\n", dgpu_fan_duty);
        last_duty_dgpu = dgpu_fan_duty = fan_smooth(last_duty_dgpu, dgpu_fan_duty);
        DCR4 = fan_max ? MAX_FAN_SPEED : dgpu_fan_duty;
        TRACE("DGPU fan_duty_smoothed=%d\n", dgpu_fan_duty);
    }
}

uint8_t fan_heatup(const struct Fan *fan, uint8_t duty) __reentrant {
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

uint8_t fan_cooldown(const struct Fan *fan, uint8_t duty) __reentrant {
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

uint8_t fan_smooth(uint8_t last_duty, uint8_t duty) __reentrant {
    uint8_t next_duty = duty;

    // ramping down
    if (duty < last_duty) {
        // out of bounds (lower) safeguard
        // clang-format off
        uint8_t smoothed = last_duty < MIN_FAN_SPEED + MAX_JUMP_DOWN
            ? MIN_FAN_SPEED
            : last_duty - MAX_JUMP_DOWN;
        // clang-format on

        // use smoothed value if above min and if smoothed is closer than raw
        if (last_duty > MIN_SPEED_TO_SMOOTH && smoothed > duty) {
            next_duty = smoothed;
        }
    }

    // ramping up
    if (duty > last_duty) {
        // out of bounds (higher) safeguard
        // clang-format off
        uint8_t smoothed = last_duty > MAX_FAN_SPEED - MAX_JUMP_UP
            ? MAX_FAN_SPEED
            : last_duty + MAX_JUMP_UP;
        // clang-format on

        // use smoothed value if above min and if smoothed is closer than raw
        if (duty > MIN_SPEED_TO_SMOOTH && smoothed < duty) {
            next_duty = smoothed;
        }
    }

    return next_duty;
}
