// SPDX-License-Identifier: GPL-3.0-only

#include <ec/pwm.h>
#include <common/macro.h>
#include <ec/gpio.h>

// TODO: Define per-board
// All boards use T0A/T1A, so just declare it here for now based on FAN2_PWM
#ifdef FAN2_PWM
#define NR_TACHS 2
#else
#define NR_TACHS 1
#endif

const enum TachCh board_tachs[NR_TACHS] = {
    TACH_CH_0A,
#ifdef FAN2_PWM
    TACH_CH_1A,
#endif
};

static void pwm_tach_init(void) {
    for (uint8_t i = 0; i < NR_TACHS; i++) {
        switch (board_tachs[i]) {
        case TACH_CH_0B:
            GCR5 |= TACH0BEN;
            TSWCTLR |= T0CHSEL;
            break;

        case TACH_CH_1B:
            GCR5 |= TACH1BEN;
            TSWCTLR |= T1CHSEL;
            break;

        case TACH_CH_2A:
            GCR2 |= TACH2AEN;
            // CHSEL default is `A` (0) at init.
            break;

#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
        case TACH_CH_2B:
            GCR15 |= TACH2BEN;
            TSWCTLR2 |= T2CHSEL;
            break;
#endif

        // XXX: SDCC 4.2.0 (Ubuntu 24.04) fails with "error 126: unreachable
        // code" if these are placed as the first cases.
        case TACH_CH_0A:
        case TACH_CH_1A:
            // T0A/T1A always available
            // CHSEL default is `A` (0) at init.
            break;
        }
    }
}

void pwm_init(void) {
    pwm_tach_init();

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

    // Turn off fans
    FAN1_PWM = 0;
#ifdef FAN2_PWM
    FAN2_PWM = 0;
#endif

#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
    // Reload counters when they reach 0 instead of immediately
    PWMLCCR = 0xFF;
#endif

    // Enable PWM
    ZTIER = BIT(1);
}
