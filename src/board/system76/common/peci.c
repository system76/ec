#include <stdbool.h>

#include <board/peci.h>
#include <board/power.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/gpio.h>
#include <ec/pwm.h>

// Fan speed is the lowest requested over HEATUP seconds
#define HEATUP 10

// Fan speed is the highest HEATUP speed over COOLDOWN seconds
#define COOLDOWN 10

// Interpolate duty cycle
#define INTERPOLATE 0

// Tjunction = 100C for i7-8565U (and probably the same for all WHL-U)
#define T_JUNCTION 100

int16_t peci_offset = 0;
int16_t peci_temp = 0;
uint8_t peci_duty = 0;

#define PECI_TEMP(X) (((int16_t)(X)) << 6)
#define PWM_DUTY(X) ((uint8_t)(((((uint16_t)(X)) * 255) + 99) / 100))

struct FanPoint {
    int16_t temp;
    uint8_t duty;
};

#define FAN_POINT(T, D) { .temp = PECI_TEMP(T), .duty = PWM_DUTY(D) }

// Fan curve with temperature in degrees C, duty cycle in percent
struct FanPoint __code FAN_POINTS[] = {
    FAN_POINT(70, 40),
    FAN_POINT(75, 50),
    FAN_POINT(80, 60),
    FAN_POINT(85, 65),
    FAN_POINT(90, 65)
};

// Get duty cycle based on temperature, adapted from
// https://github.com/pop-os/system76-power/blob/master/src/fan.rs
uint8_t fan_duty(int16_t temp) {
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

uint8_t fan_heatup(uint8_t duty) {
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

uint8_t fan_cooldown(uint8_t duty) {
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

void peci_init(void) {
    // Allow PECI pin to be used
    GCR2 |= (1 << 4);

    // Set frequency to 1MHz
    HOCTL2R = 0x01;
    // Set VTT to 1V
    PADCTLR = 0x02;
}

// PECI information can be found here: https://www.intel.com/content/dam/www/public/us/en/documents/design-guides/core-i7-lga-2011-guide.pdf
void peci_event(void) {
    if (power_state == POWER_STATE_S0) {
        // Use PECI if in S0 state

        // Wait for completion
        while (HOSTAR & 1) {}
        // Clear status
        HOSTAR = HOSTAR;

        // Enable PECI, clearing data fifo's
        HOCTLR = (1 << 5) | (1 << 3);
        // Set address to default
        HOTRADDR = 0x30;
        // Set write length
        HOWRLR = 1;
        // Set read length
        HORDLR = 2;
        // Set command
        HOCMDR = 1;
        // Start transaction
        HOCTLR |= 1;

        // Wait for completion
        while (HOSTAR & 1) {}

        if (HOSTAR & (1 << 1)) {
            // Use result if finished successfully
            uint8_t low = HORDDR;
            uint8_t high = HORDDR;
            peci_offset = ((int16_t)high << 8) | (int16_t)low;

            peci_temp = PECI_TEMP(T_JUNCTION) + peci_offset;
            peci_duty = fan_duty(peci_temp);
        } else {
            // Default to 50% if there is an error
            peci_offset = 0;
            peci_temp = 0;
            peci_duty = PWM_DUTY(50);
        }
    } else {
        // Turn fan off if not in S0 state
        peci_offset = 0;
        peci_temp = 0;
        peci_duty = PWM_DUTY(0);
    }

    uint8_t heatup_duty = fan_heatup(peci_duty);
    uint8_t cooldown_duty = fan_cooldown(heatup_duty);
    if (cooldown_duty != DCR2) {
        DCR2 = cooldown_duty;
        DEBUG("PECI offset=%d, temp=%d = %d\n", peci_offset, peci_temp, cooldown_duty);
    }
}
