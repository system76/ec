#include <string.h>
#include <avr/io.h>

#include <arch/timer.h>
#include <board/cpu.h>
#include <common/macro.h>

#define TIMER_CHANNEL(N, P) \
    { &OCR ## N ## P, _BV(COM ## N ## P ## 0), _BV(COM ## N ## P ## 1), }

#define TIMER_8(N) \
    { \
        xstr(N), \
        TIMER_8, \
        .timer_8 = { \
            &TCCR ## N ## A, \
            &TCCR ## N ## B, \
            &TCNT ## N, \
            &TIMSK ## N, \
            &TIFR ## N, \
            { \
                TIMER_CHANNEL(N, A), \
                TIMER_CHANNEL(N, B), \
            }, \
        } \
    }

#define TIMER_16(N) \
    { \
        xstr(N), \
        TIMER_16, \
        .timer_16 = { \
            &TCCR ## N ## A, \
            &TCCR ## N ## B, \
            &TCCR ## N ## C, \
            &TCNT ## N, \
            &ICR ## N, \
            &TIMSK ## N, \
            &TIFR ## N, \
            { \
                _BV(CS ## N ## 0), \
                _BV(CS ## N ## 1), \
                _BV(CS ## N ## 2), \
            }, \
            { \
                _BV(WGM ## N ## 0), \
                _BV(WGM ## N ## 1), \
                _BV(WGM ## N ## 2), \
                _BV(WGM ## N ## 3), \
            }, \
            { \
                TIMER_CHANNEL(N, A), \
                TIMER_CHANNEL(N, B), \
                TIMER_CHANNEL(N, C), \
            }, \
        } \
    }

#define TIMER_10(N) \
    { \
        xstr(N), \
        TIMER_10, \
        .timer_10 = { \
            &TCCR ## N ## A, \
            &TCCR ## N ## B, \
            &TCCR ## N ## C, \
            &TCCR ## N ## D, \
            &TCCR ## N ## E, \
            &TCNT ## N, \
            &TIMSK ## N, \
            &TIFR ## N, \
            &DT ## N, \
            { \
                _BV(CS ## N ## 0), \
                _BV(CS ## N ## 1), \
                _BV(CS ## N ## 2), \
                _BV(CS ## N ## 3), \
            }, \
            { \
                _BV(WGM ## N ## 0), \
                _BV(WGM ## N ## 1), \
            }, \
            { \
                _BV(PWM ## N ## A), \
                _BV(PWM ## N ## B), \
                _BV(PWM ## N ## D), \
            }, \
            { \
                TIMER_CHANNEL(N, A), \
                TIMER_CHANNEL(N, B), \
                { &OCR ## N ## C, 0, 0, }, \
                TIMER_CHANNEL(N, D), \
            }, \
        } \
    }

#if defined(__AVR_ATmega328P__)
    static struct Timer TIMERS[] = {
        TIMER_8(0), //Timer 0 is used by system time, it is not recommended to use it
        //TODO: missing channel 3: TIMER_16(1),
        TIMER_8(2),
    };
#elif defined(__AVR_ATmega32U4__)
    static struct Timer TIMERS[] = {
        TIMER_8(0), //Timer 0 is used by system time, it is not recommended to use it
        TIMER_16(1),
        TIMER_16(3),
        TIMER_10(4)
    };
#else
    #error "Could not find timer definitions"
#endif

int timer_count() {
    return sizeof(TIMERS)/sizeof(struct Timer);
}

struct Timer * timer_new(int num) {
    if (num < timer_count()) {
        return &TIMERS[num];
    } else {
        return NULL;
    }
}

struct Timer * timer_from_name(char * name) {
    int num;
    for (num = 0; num < timer_count(); num++) {
        struct Timer * timer = &TIMERS[num];
        if (strcmp(timer->name, name) == 0) {
            return timer;
        }
    }
    return NULL;
}

uint8_t timer_16_init(struct Timer16 * timer) {
    *(timer->tccra) = 0;
    *(timer->tccrb) = timer->cs[0];
    *(timer->tccrc) = 0;
    *(timer->tcnt) = 0;
    for (int i = 0; i < sizeof(timer->chans)/sizeof(struct Timer16Channel); i++) {
        *(timer->chans[i].ocr) = 0;
    }
    *(timer->icr) = 0;
    *(timer->timsk) = 0;
    *(timer->tifr) = *(timer->tifr);

    return 0;
}

uint8_t timer_10_init(struct Timer10 * timer) {
    *(timer->tccra) = 0;
    *(timer->tccrb) = timer->cs[0];
    *(timer->tccrc) = 0;
    *(timer->tccrd) = 0;
    *(timer->tccre) = 0;
    *(timer->tcnt) = 0;
    for (int i = 0; i < sizeof(timer->chans)/sizeof(struct Timer10Channel); i++) {
        *(timer->chans[i].ocr) = 0;
    }
    *(timer->timsk) = 0;
    *(timer->tifr) = *(timer->tifr);
    *(timer->dt) = 0;

    return 0;
}

uint8_t timer_init(struct Timer * timer) {
    switch (timer->kind) {
    case TIMER_16:
        return timer_16_init(&timer->timer_16);
    case TIMER_10:
        return timer_10_init(&timer->timer_10);
    default:
        return 1;
    }
}

uint8_t timer_16_set_frequency(struct Timer16 * timer, uint32_t frequency) {
    uint32_t top = F_CPU/frequency;
    if (top > 0xFFFF) {
        return 1;
    }

    *(timer->icr) = (uint16_t)top;

    return 0;
}

uint8_t timer_10_set_frequency(struct Timer10 * timer, uint32_t frequency) {
    uint32_t top = F_CPU/frequency;
    if (top > 0xFF) {
        return 1;
    }

    *(timer->chans[TIMER_CHANNEL_C].ocr) = (uint8_t)top;

    return 0;
}

uint8_t timer_set_frequency(struct Timer * timer, uint32_t frequency) {
    switch (timer->kind) {
    case TIMER_16:
        return timer_16_set_frequency(&timer->timer_16, frequency);
    case TIMER_10:
        return timer_10_set_frequency(&timer->timer_10, frequency);
    default:
        return 1;
    }
}

uint8_t timer_16_set_mode(struct Timer16 * timer, enum TimerMode mode) {
    uint8_t tccra = *(timer->tccra);
    uint8_t tccrb = *(timer->tccrb);

    tccra &= ~(timer->wgm[0] | timer->wgm[1]);
    tccrb &= ~(timer->wgm[2] | timer->wgm[3]);

    switch (mode) {
        case TIMER_MODE_NORMAL:
            break;
        case TIMER_MODE_CTC:
            tccrb |= timer->wgm[3] | timer->wgm[2];
            break;
        case TIMER_MODE_FAST:
            tccra |= timer->wgm[1];
            tccrb |= timer->wgm[3] | timer->wgm[2];
            break;
        case TIMER_MODE_PHASE:
            tccra |= timer->wgm[1];
            tccrb |= timer->wgm[3];
            break;
        case TIMER_MODE_PHASE_FREQ:
            tccrb |= timer->wgm[3];
            break;
        default:
            return 1;
    }

    *(timer->tccra) = tccra;
    *(timer->tccrb) = tccrb;

    return 0;
}

uint8_t timer_10_set_mode(struct Timer10 * timer, enum TimerMode mode) {
    uint8_t tccra = *(timer->tccra);
    uint8_t tccrc = *(timer->tccrc);
    uint8_t tccrd = *(timer->tccrd);

    tccra &= ~(timer->pwm[0] | timer->pwm[1]);
    tccrc &= ~(timer->pwm[2]);
    tccrd &= ~(timer->wgm[0] | timer->wgm[1]);

    switch (mode) {
        case TIMER_MODE_PHASE_FREQ:
            tccrd |= timer->wgm[0];
            //Fallthrough to set PWM pins
        case TIMER_MODE_FAST:
            tccra |= timer->pwm[0] | timer->pwm[1];
            tccrc |= timer->pwm[2];
            break;
        case TIMER_MODE_NORMAL:
            break;
        default:
            return 1;
    }

    *(timer->tccra) = tccra;
    *(timer->tccrc) = tccrc;
    *(timer->tccrd) = tccrd;

    return 0;
}

uint8_t timer_set_mode(struct Timer * timer, enum TimerMode mode) {
    switch (timer->kind) {
    case TIMER_16:
        return timer_16_set_mode(&timer->timer_16, mode);
    case TIMER_10:
        return timer_10_set_mode(&timer->timer_10, mode);
    default:
        return 1;
    }
}

uint8_t timer_channels(struct Timer * timer) {
    switch (timer->kind) {
        case TIMER_8:
            return sizeof(timer->timer_8.chans)/sizeof(struct Timer8Channel);
        case TIMER_16:
            return sizeof(timer->timer_16.chans)/sizeof(struct Timer16Channel);
        case TIMER_10:
            return sizeof(timer->timer_10.chans)/sizeof(struct Timer10Channel);
        default:
            return 0;
    }
}

uint8_t timer_16_set_channel_duty(struct Timer16 * timer, enum TimerChannel channel, uint16_t duty) {
    if (channel >= sizeof(timer->chans)/sizeof(struct Timer16Channel)) {
        return 1;
    }

    struct Timer16Channel * chan = &timer->chans[channel];

    uint32_t top = (uint32_t)(*(timer->icr));
    uint32_t bottom = ((top * ((uint32_t)duty)) / TIMER_DUTY_MAX);
    if (bottom > top) {
        bottom = top;
    }

    *(chan->ocr) = (uint16_t)bottom;

    return 0;
}

uint8_t timer_10_set_channel_duty(struct Timer10 * timer, enum TimerChannel channel, uint16_t duty) {
    if (channel >= sizeof(timer->chans)/sizeof(struct Timer10Channel)) {
        return 1;
    }

    struct Timer10Channel * chan = &timer->chans[channel];

    uint32_t top = (uint32_t)(*(timer->chans[TIMER_CHANNEL_C].ocr));
    uint32_t bottom = ((top * ((uint32_t)duty)) / TIMER_DUTY_MAX);
    if (bottom > top) {
        bottom = top;
    }

    *(chan->ocr) = (uint8_t)bottom;

    return 0;
}

uint8_t timer_set_channel_duty(struct Timer * timer, enum TimerChannel channel, uint16_t duty) {
    switch (timer->kind) {
        case TIMER_16:
            return timer_16_set_channel_duty(&timer->timer_16, channel, duty);
        case TIMER_10:
            return timer_10_set_channel_duty(&timer->timer_10, channel, duty);
        default:
            return 1;
    }
}

uint8_t timer_16_set_channel_mode(struct Timer16 * timer, enum TimerChannel channel, enum TimerChannelMode mode) {
    if (channel >= sizeof(timer->chans)/sizeof(struct Timer16Channel)) {
        return 1;
    }

    struct Timer16Channel * chan = &timer->chans[channel];

    uint8_t tccra = *(timer->tccra);

    if (mode & 0b01) {
        tccra |= chan->com0;
    } else {
        tccra &= ~(chan->com0);
    }

    if (mode & 0b10) {
        tccra |= chan->com1;
    } else {
        tccra &= ~(chan->com1);
    }

    *(timer->tccra) = tccra;

    return 0;
}

uint8_t timer_10_set_channel_mode(struct Timer10 * timer, enum TimerChannel channel, enum TimerChannelMode mode) {
    if (channel >= sizeof(timer->chans)/sizeof(struct Timer10Channel)) {
        return 1;
    }

    struct Timer10Channel * chan = &timer->chans[channel];

    uint8_t tccra = *(timer->tccra);

    if (mode & 0b01) {
        tccra |= chan->com0;
    } else {
        tccra &= ~(chan->com0);
    }

    if (mode & 0b10) {
        tccra |= chan->com1;
    } else {
        tccra &= ~(chan->com1);
    }

    *(timer->tccra) = tccra;

    return 0;
}

uint8_t timer_set_channel_mode(struct Timer * timer, enum TimerChannel channel, enum TimerChannelMode mode) {
    if (channel >= timer_channels(timer)) {
        return 1;
    }

    switch (timer->kind) {
        case TIMER_16:
            return timer_16_set_channel_mode(&timer->timer_16, channel, mode);
        case TIMER_10:
            return timer_10_set_channel_mode(&timer->timer_10, channel, mode);
        default:
            return 1;
    }
}
