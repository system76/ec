// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>

bool fan_max = false;
#define max_speed PWM_DUTY(100)
#define min_speed PWM_DUTY(0)

#ifdef FAN_SMOOTHING
  const uint8_t max_jump = (max_speed - min_speed) / (uint8_t) FAN_SMOOTHING;
#endif

void fan_reset(void) {
    // Do not manually set fans to maximum speed
    fan_max = false;
}

// Get duty cycle based on temperature, adapted from
// https://github.com/pop-os/system76-power/blob/master/src/fan.rs
uint8_t fan_duty(const struct Fan * fan, int16_t temp) __reentrant {
    for (int i = 0; i < fan->points_size; i++) {
        const struct FanPoint * cur = &fan->points[i];

        // If exactly the current temp, return the current duty
        if (temp == cur->temp) {
            return cur->duty;
        } else if (temp < cur->temp) {
            // If lower than first temp, return 0%
            if (i == 0) {
                return min_speed;
            } else {
                const struct FanPoint * prev = &fan->points[i - 1];

                if (fan->interpolate) {
                    // If in between current temp and previous temp, interpolate
                    if (temp > prev->temp) {
                        int16_t dtemp = (cur->temp - prev->temp);
                        int16_t dduty = ((int16_t)cur->duty) - ((int16_t)prev->duty);
                        return (uint8_t)(
                            ((int16_t)prev->duty) +
                            ((temp - prev->temp) * dduty) / dtemp
                        );
                    }
                } else {
                    return prev->duty;
                }
            }
        }
    }

    // If no point is found, return 100%
    return max_speed;
}

uint8_t fan_heatup(const struct Fan * fan, uint8_t duty) __reentrant {
    uint8_t lowest = duty;

    int i;
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

uint8_t fan_cooldown(const struct Fan * fan, uint8_t duty) __reentrant {
    uint8_t highest = duty;

    int i;
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

#ifdef FAN_SMOOTHING
  uint8_t fan_smooth(uint8_t last_duty, uint8_t duty) __reentrant {
    uint8_t next_duty = duty;

    if (last_duty > duty) {
      uint8_t smoothed = last_duty < min_speed + max_jump
        ? min_speed
        : last_duty - max_jump;

      if (smoothed > duty) {
        next_duty = smoothed;
      }
    }

    if (last_duty < duty) {
      uint8_t smoothed = last_duty > max_speed - max_jump
        ? max_speed
        : last_duty + max_jump;

      if (smoothed < duty) {
        next_duty = smoothed;
      }
    }

    return next_duty;
  }
#endif
