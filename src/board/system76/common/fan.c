// SPDX-License-Identifier: GPL-3.0-only

#include <board/fan.h>

bool fan_max = false;
#define max_speed PWM_DUTY(100)
#define min_speed PWM_DUTY(0)

#if !defined(FAN_SMOOTHING) && (defined(FAN_SMOOTHING_UP) || defined(FAN_SMOOTHING_DOWN))
  #define FAN_SMOOTHING 40
#endif

#ifdef FAN_SMOOTHING
  #ifndef FAN_SMOOTHING_UP
    #define FAN_SMOOTHING_UP FAN_SMOOTHING
  #endif
  #ifndef FAN_SMOOTHING_DOWN
    #define FAN_SMOOTHING_DOWN FAN_SMOOTHING
  #endif

  const uint8_t max_jump_up = (max_speed - min_speed) / (uint8_t) FAN_SMOOTHING_UP;
  const uint8_t max_jump_down = (max_speed - min_speed) / (uint8_t) FAN_SMOOTHING_DOWN;
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

    // ramping down
    if (duty < last_duty) {
      // out of bounds (lower) safeguard
      uint8_t smoothed = last_duty < min_speed + max_jump_down
        ? min_speed
        : last_duty - max_jump_down;

      if (smoothed > duty) {
        next_duty = smoothed;
      }
    }

    // ramping up
    if (duty > last_duty) {
      // out of bounds (higher) safeguard
      uint8_t smoothed = last_duty > max_speed - max_jump_up
        ? max_speed
        : last_duty + max_jump_up;

      if (smoothed < duty) {
        next_duty = smoothed;
      }
    }

    return next_duty;
  }
#endif
