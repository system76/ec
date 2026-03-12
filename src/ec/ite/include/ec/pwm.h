// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_PWM_H
#define _EC_PWM_H

#include <stdint.h>

// Channel 0 clock prescaler register
volatile uint8_t __xdata __at(0x1800) C0CPRS;
// Channel 6 clock prescaler register (low byte)
volatile uint8_t __xdata __at(0x182B) C6CPRS;
// Channel 6 clock prescaler register (high byte)
volatile uint8_t __xdata __at(0x182C) C6MCPRS;

// Cycle Time 0
volatile uint8_t __xdata __at(0x1801) CTR0;
// Cycle Time 1
volatile uint8_t __xdata __at(0x1841) CTR1;
// Cycle Time 2
volatile uint8_t __xdata __at(0x1842) CTR2;
// Cycle Time 3
volatile uint8_t __xdata __at(0x1843) CTR3;

// Duty cycle register 0
volatile uint8_t __xdata __at(0x1802) DCR0;
// Duty cycle register 1
volatile uint8_t __xdata __at(0x1803) DCR1;
// Duty cycle register 2
volatile uint8_t __xdata __at(0x1804) DCR2;
// Duty cycle register 3
volatile uint8_t __xdata __at(0x1805) DCR3;
// Duty cycle register 4
volatile uint8_t __xdata __at(0x1806) DCR4;
// Duty cycle register 5
volatile uint8_t __xdata __at(0x1807) DCR5;
// Duty cycle register 6
volatile uint8_t __xdata __at(0x1808) DCR6;
// Duty cycle register 7
volatile uint8_t __xdata __at(0x1809) DCR7;

// Fan one tachometer least significant byte reading register
volatile uint8_t __xdata __at(0x181E) F1TLRR;
// Fan one tachometer most significant byte reading register
volatile uint8_t __xdata __at(0x181F) F1TMRR;
// Fan two tachometer least significant byte reading register
volatile uint8_t __xdata __at(0x1820) F2TLRR;
// Fan two tachometer most significant byte reading register
volatile uint8_t __xdata __at(0x1821) F2TMRR;

// PWM polarity register
volatile uint8_t __xdata __at(0x180A) PWMPOL;
// Prescaler clock frequency select register
volatile uint8_t __xdata __at(0x180B) PCFSR;
// Prescaler clock source select group low
volatile uint8_t __xdata __at(0x180C) PCSSGL;
// Prescaler clock source select group high
volatile uint8_t __xdata __at(0x180D) PCSSGH;
// Prescaler clock source gating register
volatile uint8_t __xdata __at(0x180F) PCSGR;
// PWM clock control register with weird name
volatile uint8_t __xdata __at(0x1823) ZTIER;
// Tachometer switch control register
volatile uint8_t __xdata __at(0x1848) TSWCTLR;
#define T1CHSEL     BIT(0)
#define T1DVS       BIT(1)
#define T0CHSEL     BIT(2)
#define T0DVS       BIT(3)

volatile uint8_t __xdata __at(0x1845) F3TLRR; // CFLRR on IT8587
volatile uint8_t __xdata __at(0x1846) F3TMRR; // CFMRR on IT8587

#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
// Tachometer switch control register 2
volatile uint8_t __xdata __at(0x184F) TSWCTLR2;
#define T2CHSEL     BIT(0)
#define T2DVS       BIT(1)

// PWM Load Counter Control Register
volatile uint8_t __xdata __at(0x185A) PWMLCCR;
#endif

enum TachCh {
    TACH_CH_0A = 0, // GPD6
    TACH_CH_1A, // GPD7
    TACH_CH_0B, // GPJ2
    TACH_CH_1B, // GPJ3
    TACH_CH_2A, // GPJ0
#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
    TACH_CH_2B, // GPJ1
#endif
};

void pwm_init(void);

#endif // _EC_PWM_H
