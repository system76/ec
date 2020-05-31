#include <stdbool.h>
#include <stdint.h>

#include <board/power.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/i2c.h>
#include <ec/pwm.h>

// Fan speed is the lowest requested over HEATUP seconds
#define HEATUP 10

// Fan speed is the highest HEATUP speed over COOLDOWN seconds
#define COOLDOWN 10

// Interpolate duty cycle
#define INTERPOLATE 0

int16_t dgpu_temp = 0;
uint8_t dgpu_duty = 0;

#define DGPU_TEMP(X) ((int16_t)(X))
#define PWM_DUTY(X) ((uint8_t)(((((uint16_t)(X)) * 255) + 99) / 100))

struct FanPoint {
    int16_t temp;
    uint8_t duty;
};

#define FAN_POINT(T, D) { .temp = DGPU_TEMP(T), .duty = PWM_DUTY(D) }

// Fan curve with temperature in degrees C, duty cycle in percent
static struct FanPoint __code FAN_POINTS[] = {
    FAN_POINT(70, 40),
    FAN_POINT(75, 50),
    FAN_POINT(80, 60),
    FAN_POINT(85, 65),
    FAN_POINT(90, 65)
};

// Get duty cycle based on temperature, adapted from
// https://github.com/pop-os/system76-power/blob/master/src/fan.rs
static uint8_t fan_duty(int16_t temp) {
    for (int i = 0; i < ARRAY_SIZE(FAN_POINTS); i++) {
        const struct FanPoint * cur = &FAN_POINTS[i];

        // If exactly the current temp, return the current duty
        if (temp == cur->temp) {
            return cur->duty;
        } else if (temp < cur->temp) {
            // If lower than first temp, return 0%
            if (i == 0) {
                return PWM_DUTY(0);
            } else {
                const struct FanPoint * prev = &FAN_POINTS[i - 1];

#if INTERPOLATE
                // If in between current temp and previous temp, interpolate
                if (temp > prev->temp) {
                    int16_t dtemp = (cur->temp - prev->temp);
                    int16_t dduty = ((int16_t)cur->duty) - ((int16_t)prev->duty);
                    return (uint8_t)(
                        ((int16_t)prev->duty) +
                        ((temp - prev->temp) * dduty) / dtemp
                    );
                }
#else // INTERPOLATE
                return prev->duty;
#endif // INTERPOLATE
            }
        }
    }

    // If no point is found, return 100%
    return PWM_DUTY(100);
}

static uint8_t fan_heatup(uint8_t duty) {
    static uint8_t history[HEATUP] = { 0 };
    uint8_t lowest = duty;

    int i;
    for (i = 0; (i + 1) < ARRAY_SIZE(history); i++) {
        uint8_t value = history[i + 1];
        if (value < lowest) {
            lowest = value;
        }
        history[i] = value;
    }
    history[i] = duty;

    return lowest;
}

static uint8_t fan_cooldown(uint8_t duty) {
    static uint8_t history[COOLDOWN] = { 0 };
    uint8_t highest = duty;

    int i;
    for (i = 0; (i + 1) < ARRAY_SIZE(history); i++) {
        uint8_t value = history[i + 1];
        if (value > highest) {
            highest = value;
        }
        history[i] = value;
    }
    history[i] = duty;

    return highest;
}

void dgpu_init(void) {
    // Set up for i2c usage
    i2c_reset(&I2C_DGPU, true);
}

void dgpu_event(void) {
    if (power_state == POWER_STATE_S0) {
        // Use I2CS if in S0 state

        int8_t rlts;
        int res = i2c_get(&I2C_DGPU, 0x4F, 0x00, &rlts, 1);
        if (res == 1) {
            dgpu_temp = (int16_t)rlts;
            dgpu_duty = fan_duty(dgpu_temp);
        } else {
            DEBUG("DGPU temp error: %d\n", res);
            // Default to 50% if there is an error
            dgpu_temp = 0;
            dgpu_duty = PWM_DUTY(50);
        }
    } else {
        // Turn fan off if not in S0 state
        dgpu_temp = 0;
        dgpu_duty = PWM_DUTY(0);
    }

    uint8_t heatup_duty = fan_heatup(dgpu_duty);
    uint8_t cooldown_duty = fan_cooldown(heatup_duty);
    if (cooldown_duty != DCR4) {
        DCR4 = cooldown_duty;
        DEBUG("DGPU temp=%d = %d\n", dgpu_temp, cooldown_duty);
    }
}
