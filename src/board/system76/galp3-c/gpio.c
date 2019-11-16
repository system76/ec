#include <stdio.h>

#include <board/gpio.h>

void gpio_init() {
    // Enable LPC reset on GPD2
    GCR = 0x04;

    // Set GPIO data
    GPDRA = 0;
    GPDRB = (1 << 0);
    GPDRC = 0;
    GPDRD = (1 << 5) | (1 << 4) | (1 << 3);
    GPDRE = 0;
    GPDRF = 0xC0; // (1 << 7) | (1 << 6)
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
    GPCRF4 = 0x00;
    GPCRF5 = 0x00;
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
    GPCRM0 = 0x00;
    GPCRM1 = 0x00;
    GPCRM2 = 0x00;
    GPCRM3 = 0x00;
    GPCRM4 = 0x00;
    GPCRM5 = 0x00;
    GPCRM6 = 0x00;
}

void gpio_debug_bank(
    char * bank,
    uint8_t data,
    uint8_t mirror,
    uint8_t pot,
    volatile uint8_t * control
) {
    for(char i = 0; i < 8; i++) {
        printf(
            "%s%d:\n\tdata %d\n\tmirror %d\n\tpot %d\n\tcontrol %02X\n",
            bank,
            i,
            (data >> i) & 1,
            (mirror >> i) & 1,
            (pot >> i) & 1,
            *(control + i)
        );
    }
}

void gpio_debug(void) {
    #define bank(BANK) gpio_debug_bank(#BANK, GPDR ## BANK, GPDMR ## BANK, GPOT ## BANK, &GPCR ## BANK ## 0)
    bank(A);
    bank(B);
    bank(C);
    bank(D);
    bank(E);
    bank(F);
    bank(G);
    bank(H);
    bank(I);
    bank(J);
    #undef bank
}
