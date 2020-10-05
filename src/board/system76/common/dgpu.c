// SPDX-License-Identifier: GPL-3.0-only

#include <board/dgpu.h>

#if HAVE_DGPU

#include <board/fan.h>
#include <board/gpio.h>
#include <board/power.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/i2c.h>
#include <ec/pwm.h>

// Fan speed is the lowest requested over HEATUP seconds
#ifndef BOARD_DGPU_HEATUP
    #define BOARD_DGPU_HEATUP 10
#endif

static uint8_t FAN_HEATUP[BOARD_DGPU_HEATUP] = { 0 };

// Fan speed is the highest HEATUP speed over COOLDOWN seconds
#ifndef BOARD_DGPU_COOLDOWN
    #define BOARD_DGPU_COOLDOWN 10
#endif

static uint8_t FAN_COOLDOWN[BOARD_DGPU_COOLDOWN] = { 0 };

int16_t dgpu_temp = 0;

#define DGPU_TEMP(X) ((int16_t)(X))

#define FAN_POINT(T, D) { .temp = DGPU_TEMP(T), .duty = PWM_DUTY(D) }

// Fan curve with temperature in degrees C, duty cycle in percent
static struct FanPoint __code FAN_POINTS[] = {
#ifdef BOARD_DGPU_FAN_POINTS
    BOARD_DGPU_FAN_POINTS
#else
    FAN_POINT(70, 40),
    FAN_POINT(75, 50),
    FAN_POINT(80, 60),
    FAN_POINT(85, 65),
    FAN_POINT(90, 65)
#endif
};

static struct Fan __code FAN = {
    .points = FAN_POINTS,
    .points_size = ARRAY_SIZE(FAN_POINTS),
    .heatup = FAN_HEATUP,
    .heatup_size = ARRAY_SIZE(FAN_HEATUP),
    .cooldown = FAN_COOLDOWN,
    .cooldown_size = ARRAY_SIZE(FAN_COOLDOWN),
    .interpolate = false,
};

void dgpu_init(void) {
    // Set up for i2c usage
    i2c_reset(&I2C_DGPU, true);
}

void dgpu_event(void) {
    uint8_t duty;
    if (power_state == POWER_STATE_S0 && gpio_get(&DGPU_PWR_EN) && !gpio_get(&GC6_FB_EN)) {
        // Use I2CS if in S0 state
        int8_t rlts;
        int res = i2c_get(&I2C_DGPU, 0x4F, 0x00, &rlts, 1);
        if (res == 1) {
            dgpu_temp = (int16_t)rlts;
            duty = fan_duty(&FAN, dgpu_temp);
        } else {
            DEBUG("DGPU temp error: %d\n", res);
            // Default to 50% if there is an error
            dgpu_temp = 0;
            duty = PWM_DUTY(50);
        }
    } else {
        // Turn fan off if not in S0 state or GPU power not on
        dgpu_temp = 0;
        duty = PWM_DUTY(0);
    }

    if (fan_max) {
        // Override duty if fans are manually set to maximum
        duty = PWM_DUTY(100);
    } else {
        // Apply heatup and cooldown filters to duty
        duty = fan_heatup(&FAN, duty);
        duty = fan_cooldown(&FAN, duty);
    }

    if (duty != DCR4) {
        DCR4 = duty;
        DEBUG("DGPU temp=%d = %d\n", dgpu_temp, duty);
    }
}

#else

void dgpu_init(void) {}

void dgpu_event(void) {}

#endif // HAVE_DGPU
