#include <stdio.h>

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

void gpio_debug_bank(char bank, unsigned char data) {
    for(char i = 0; i < 8; i++) {
        printf("%c%d = %d\n", bank, i, (data >> i) & 1);
    }
}

void gpio_debug(void) {
    gpio_debug_bank('A', GPDRA);
    gpio_debug_bank('B', GPDRB);
    gpio_debug_bank('C', GPDRC);
    gpio_debug_bank('D', GPDRD);
    gpio_debug_bank('E', GPDRE);
    gpio_debug_bank('F', GPDRF);
    gpio_debug_bank('G', GPDRG);
    gpio_debug_bank('H', GPDRH);
    gpio_debug_bank('I', GPDRI);
    gpio_debug_bank('J', GPDRJ);
}
