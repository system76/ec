// SPDX-License-Identifier: GPL-3.0-only

#ifndef _ARCH_TIMER_H
#define _ARCH_TIMER_H

#include <stdint.h>

enum TimerKind {
    TIMER_8 = 8,
    TIMER_16 = 16,
    TIMER_10 = 10,
};

struct Timer8Channel {
    volatile uint8_t * ocr;
    uint8_t com0;
    uint8_t com1;
};

struct Timer8 {
    volatile uint8_t * tccra;
    volatile uint8_t * tccrb;
    volatile uint8_t * tcnt;
    volatile uint8_t * timsk;
    volatile uint8_t * tifr;
    struct Timer8Channel chans[2];
};

struct Timer16Channel {
    volatile uint16_t * ocr;
    uint8_t com0;
    uint8_t com1;
};

struct Timer16 {
    volatile uint8_t * tccra;
    volatile uint8_t * tccrb;
    volatile uint8_t * tccrc;
    volatile uint16_t * tcnt;
    volatile uint16_t * icr;
    volatile uint8_t * timsk;
    volatile uint8_t * tifr;
    uint8_t cs[3];
    uint8_t wgm[4];
    struct Timer16Channel chans[3];
};

struct Timer10Channel {
    volatile uint8_t * ocr;
    uint8_t com0;
    uint8_t com1;
};

struct Timer10 {
    volatile uint8_t * tccra;
    volatile uint8_t * tccrb;
    volatile uint8_t * tccrc;
    volatile uint8_t * tccrd;
    volatile uint8_t * tccre;
    volatile uint16_t * tcnt;
    volatile uint8_t * timsk;
    volatile uint8_t * tifr;
    volatile uint8_t * dt;
    uint8_t cs[4];
    uint8_t wgm[2];
    uint8_t pwm[3];
    struct Timer10Channel chans[4];
};

struct Timer {
    char * name;
    enum TimerKind kind;
    union {
        struct Timer8 timer_8;
        struct Timer16 timer_16;
        struct Timer10 timer_10;
    };
};

enum TimerMode {
    TIMER_MODE_NORMAL,
    TIMER_MODE_CTC,
    TIMER_MODE_FAST,
    TIMER_MODE_PHASE,
    TIMER_MODE_PHASE_FREQ,
};

enum TimerChannel {
    TIMER_CHANNEL_A = 0,
    TIMER_CHANNEL_B = 1,
    TIMER_CHANNEL_C = 2,
    TIMER_CHANNEL_D = 3,
};

enum TimerChannelMode {
    TIMER_CHANNEL_MODE_NORMAL = 0b00,
    TIMER_CHANNEL_MODE_TOGGLE = 0b01,
    TIMER_CHANNEL_MODE_CLEAR = 0b10,
    TIMER_CHANNEL_MODE_SET = 0b11,
};

#define TIMER_DUTY_MAX 10000UL

int timer_count();
struct Timer * timer_new(int num);
struct Timer * timer_from_name(char * name);

uint8_t timer_init(struct Timer * timer);
uint8_t timer_set_frequency(struct Timer * timer, uint32_t frequency);
uint8_t timer_set_mode(struct Timer * timer, enum TimerMode mode);

uint8_t timer_channels(struct Timer * timer);
uint8_t timer_set_channel_duty(struct Timer * timer, enum TimerChannel channel, uint16_t duty);
uint8_t timer_set_channel_mode(struct Timer * timer, enum TimerChannel channel, enum TimerChannelMode mode);

#endif // _ARCH_TIMER_H
