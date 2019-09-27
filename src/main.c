#include "include/signature.h"

void reset() {
    __asm__("ljmp 0");
}

#include "include/gpio.h"

void gpio_init() {
    // Enable LPC reset on GPD2
    GCR = 0x04;

    // Set GPIO data
    GPDRA = 0;
    GPDRB = (1 << 0);
    GPDRC = 0;
    GPDRD = (1 << 5) | (1 << 4) | (1 << 3);
    GPDRE = 0;
    GPDRF = (1 << 7) | (1 << 6);
    GPDRG = 0;
    GPDRH = 0;
    GPDRI = 0;
    GPDRJ = 0;

    // Set GPIO control
    GPCRA0 = 0x80;
    GPCRA1 = 0x00;
    GPCRA2 = 0x00;
    GPCRA3 = 0x80;
    GPCRA4 = 0x40;
    GPCRA5 = 0x44;
    GPCRA6 = 0x44;
    GPCRA7 = 0x44;
    GPCRB0 = 0x44;
    GPCRB1 = 0x44;
    GPCRB2 = 0x84;
    GPCRB3 = 0x00;
    GPCRB4 = 0x00;
    GPCRB5 = 0x44;
    GPCRB6 = 0x84;
    GPCRB7 = 0x80;
    GPCRC0 = 0x80;
    GPCRC1 = 0x84;
    GPCRC2 = 0x84;
    GPCRC3 = 0x84;
    GPCRC4 = 0x44;
    GPCRC5 = 0x44;
    GPCRC6 = 0x40;
    GPCRC7 = 0x44;
    GPCRD0 = 0x84;
    GPCRD1 = 0x84;
    GPCRD2 = 0x00;
    GPCRD3 = 0x80;
    GPCRD4 = 0x80;
    GPCRD5 = 0x44;
    GPCRD6 = 0x80;
    GPCRD7 = 0x80;
    GPCRE0 = 0x44;
    GPCRE1 = 0x44;
    GPCRE2 = 0x80;
    GPCRE3 = 0x40;
    GPCRE4 = 0x42;
    GPCRE5 = 0x40;
    GPCRE6 = 0x44;
    GPCRE7 = 0x44;
    GPCRF0 = 0x80;
    GPCRF1 = 0x44;
    GPCRF2 = 0x84;
    GPCRF3 = 0x44;
    GPCRF4 = 0x80;
    GPCRF5 = 0x80;
    GPCRF6 = 0x00;
    GPCRF7 = 0x80;
    GPCRG0 = 0x44;
    GPCRG1 = 0x44;
    GPCRG2 = 0x40;
    GPCRG3 = 0x00;
    GPCRG4 = 0x00;
    GPCRG5 = 0x00;
    GPCRG6 = 0x44;
    GPCRG7 = 0x00;
    GPCRH0 = 0x00;
    GPCRH1 = 0x80;
    GPCRH2 = 0x44;
    GPCRH3 = 0x44;
    GPCRH4 = 0x80;
    GPCRH5 = 0x80;
    GPCRH6 = 0x80;
    GPCRH7 = 0x80;
    GPCRI0 = 0x00;
    GPCRI1 = 0x00;
    GPCRI2 = 0x80;
    GPCRI3 = 0x00;
    GPCRI4 = 0x00;
    GPCRI5 = 0x80;
    GPCRI6 = 0x80;
    GPCRI7 = 0x80;
    GPCRJ0 = 0x82;
    GPCRJ1 = 0x80;
    GPCRJ2 = 0x40;
    GPCRJ3 = 0x80;
    GPCRJ4 = 0x44;
    GPCRJ5 = 0x40;
    GPCRJ6 = 0x44;
    GPCRJ7 = 0x80;
}

#include "include/gctrl.h"

void gctrl_init() {
    SPCTRL1 = 0x03;
    BADRSEL = 0;
    RSTS = 0x84;
}

#include "include/kbc.h"

void kbc_init() {
    KBIRQR = 0;
    KBHICR = 0x48;
}

#include "include/pmc.h"

void pmc_init() {
    PM1CTL = 0x41;
    PM2CTL = 0x41;
}

#include "include/ps2.h"

void ps2_init() {
    PSCTL1 = 0x11;
    PSCTL2 = 0x41;
    PSCTL3 = 0x41;
    PSINT1 = 0x04;
    PSINT2 = 0x04;
    PSINT3 = 0x04;
}

#include "include/kbscan.h"

void kbscan_init() {
    KSOCTRL = 0x05;
    KSICTRLR = 0x04;

    // Set all outputs to GPIO mode and high
    KSOH2 = 0xFF;
    KSOH1 = 0xFF;
    KSOL = 0xFF;
    KSOHGCTRL = 0xFF;
    KSOHGOEN = 0xFF;
    KSOLGCTRL = 0xFF;
    KSOLGOEN = 0xFF;
}

#include <8051.h>

void timer_clear(void) {
    TR0 = 0;
    TF0 = 0;
}

void timer_mode_1(int value) {
    timer_clear();
    TMOD = 0x01;
    TH0 = (unsigned char)(value >> 8);
    TL0 = (unsigned char)value;
    TR0 = 1;
}

void delay_ms(int ms) {
    for (int i = 0; i < ms; i++) {
        // One millisecond in ticks is determined as follows:
        //   9.2 MHz is the clock rate
        //   The timer divider is 12
        //   The timer rate is 12 / 9.2 MHz = 1.304 us
        //   The ticks are 1000 ms / (1.304 us) = 766.667
        //   65536 - 766.667 = 64769.33
        timer_mode_1(64769);
        while (TF0 == 0) {}
        timer_clear();
    }
}

#include <stdbool.h>

struct Pin {
    __xdata volatile unsigned char * data;
    __xdata volatile unsigned char * mirror;
    __xdata volatile unsigned char * control;
    unsigned char value;
};

#define PIN(BLOCK, NUMBER) { \
    .data = &GPDR ## BLOCK, \
    .mirror = &GPDMR ## BLOCK, \
    .control = &GPCR ## BLOCK ## NUMBER, \
    .value = (1 << NUMBER), \
}

bool pin_get(struct Pin * pin) {
    if (*(pin->data) & pin->value) {
        return true;
    } else {
        return false;
    }
}

void pin_set(struct Pin * pin, bool value) {
    if (value) {
        *(pin->data) |= pin->value;
    } else {
        *(pin->data) &= ~(pin->value);
    }
}

struct Pin LED_BAT_CHG = PIN(A, 5);
struct Pin LED_BAT_FULL = PIN(A, 6);
struct Pin LED_PWR = PIN(A, 7);
struct Pin LED_ACIN = PIN(C, 7);
struct Pin LED_AIRPLANE_N = PIN(G, 6);

struct Pin PWR_SW = PIN(D, 0);

void parallel_write(unsigned char value) {
    // Make sure clock is high
    KSOH1 = 0xFF;
    delay_ms(1);

    // Set value
    KSOL = value;
    delay_ms(1);

    // Set clock low
    KSOH1 = 0;
    pin_set(&LED_ACIN, true);
    delay_ms(1);

    // Set clock high again
    pin_set(&LED_ACIN, false);
    KSOH1 = 0xFF;
}

void puts(const char * s) {
    char c;
    while (c = *(s++)) {
        parallel_write((unsigned char)c);
    }
}

void main() {
    gpio_init();

    gctrl_init();

    kbc_init();

    pmc_init();

    kbscan_init();

    //TODO: INTC, PECI, PWM, SMBUS

    // Set the battery full LED (to know our firmware is loaded)
    pin_set(&LED_BAT_CHG, true);
    delay_ms(1000);
    pin_set(&LED_BAT_FULL, true);
    puts("Hello from System76 EC!\n");

    bool last = false;
    for(;;) {
        // Check if the power switch goes low
        bool new = pin_get(&PWR_SW);
        if (!new && last) {
            puts("Power Switch\n");
        }
        last = new;
    }
}
