// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/debug.h>

struct Gpio __code ACIN_N =         GPIO(B, 0);
struct Gpio __code AC_PRESENT =     GPIO(E, 1);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(H, 2);
struct Gpio __code BT_EN =          GPIO(F, 3);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2);
struct Gpio __code CCD_EN =         GPIO(D, 1);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code EC_EN =          GPIO(J, 6);
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code EC_SMD_EN_N =    GPIO(I, 6);
struct Gpio __code LED_ACIN =       GPIO(C, 7);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code PCH_DPWROK_EC =  GPIO(A, 7);
struct Gpio __code PCH_PWROK_EC =   GPIO(A, 6);
struct Gpio __code PM_CLKRUN_N =    GPIO(H, 0);
struct Gpio __code PM_PWROK =       GPIO(C, 6);
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code SB_KBCRST_N =    GPIO(E, 6);
struct Gpio __code SCI_N =          GPIO(D, 3);
struct Gpio __code SLP_SUS_N =      GPIO(J, 3);
struct Gpio __code SMI_N =          GPIO(D, 4);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code SUSWARN_N =      GPIO(D, 7);
struct Gpio __code SUS_PWR_ACK =    GPIO(J, 7);
struct Gpio __code SWI_N =          GPIO(B, 5);
struct Gpio __code USB_PWR_EN_N =   GPIO(E, 3);
struct Gpio __code VA_EC_EN =       GPIO(J, 4);
struct Gpio __code VR_ON =          GPIO(H, 4);
struct Gpio __code WLAN_EN =        GPIO(G, 1);
struct Gpio __code WLAN_PWR_EN =    GPIO(A, 3);
struct Gpio __code XLP_OUT =        GPIO(B, 4);

void gpio_init() {
    // Enable LPC reset on GPD2
    GCR = 0x04;
    // Set GPF2 and GPF3 to 3.3V
    GCR20 = 0;
    // Enable SMBus channel 4
    GCR15 = (1 << 4);
    // Set GPD2 to 1.8V
    GCR19 = (1 << 0);
    // Set GPH0 to 1.8V
    GCR21 = (1 << 2);

    // Set GPIO data
    GPDRA = 0x00;
    GPDRB = 0x18;
    GPDRC = 0x20;
    GPDRD = 0x10;
    GPDRE = 0x08;
    GPDRF = 0x40;
    GPDRG = 0x00;
    GPDRH = 0x00;
    GPDRI = 0x00;
    GPDRJ = 0x00;
    GPDRM = 0x00;

    // Set GPIO control
    GPCRA0 = GPIO_OUT;
    GPCRA1 = GPIO_ALT;
    GPCRA2 = GPIO_ALT;
    GPCRA3 = GPIO_OUT | GPIO_UP;
    GPCRA4 = GPIO_ALT;
    GPCRA5 = GPIO_IN;
    GPCRA6 = GPIO_OUT | GPIO_UP;
    GPCRA7 = GPIO_IN;
    GPCRB0 = GPIO_IN | GPIO_UP;
    GPCRB1 = GPIO_IN | GPIO_UP;
    GPCRB2 = GPIO_IN | GPIO_UP;
    GPCRB3 = GPIO_IN;
    GPCRB4 = GPIO_OUT;
    GPCRB5 = GPIO_OUT | GPIO_UP;
    GPCRB6 = GPIO_OUT | GPIO_UP;
    GPCRB7 = GPIO_IN;
    GPCRC0 = GPIO_IN;
    GPCRC1 = GPIO_ALT | GPIO_UP;
    GPCRC2 = GPIO_ALT | GPIO_UP;
    GPCRC3 = GPIO_IN;
    GPCRC4 = GPIO_IN | GPIO_UP;
    GPCRC5 = GPIO_OUT | GPIO_UP;
    GPCRC6 = GPIO_OUT;
    GPCRC7 = GPIO_OUT | GPIO_UP;
    GPCRD0 = GPIO_OUT | GPIO_UP;
    GPCRD1 = GPIO_OUT | GPIO_UP;
    GPCRD2 = GPIO_ALT;
    GPCRD3 = GPIO_IN | GPIO_DOWN;
    GPCRD4 = GPIO_IN;
    GPCRD5 = GPIO_OUT | GPIO_UP;
    GPCRD6 = GPIO_IN;
    GPCRD7 = GPIO_IN;
    GPCRE0 = GPIO_ALT;
    GPCRE1 = GPIO_OUT | GPIO_UP;
    GPCRE2 = GPIO_IN | GPIO_UP;
    GPCRE3 = GPIO_OUT | GPIO_UP;
    GPCRE4 = GPIO_OUT | GPIO_DOWN;
    GPCRE5 = GPIO_OUT;
    GPCRE6 = GPIO_IN;
    GPCRE7 = GPIO_ALT;
    GPCRF0 = GPIO_IN;
    GPCRF1 = GPIO_OUT | GPIO_UP;
    GPCRF2 = GPIO_IN | GPIO_UP;
    GPCRF3 = 0x06;
    GPCRF4 = GPIO_IN;
    GPCRF5 = GPIO_IN;
    GPCRF6 = GPIO_ALT;
    GPCRF7 = GPIO_OUT | GPIO_UP;
    GPCRG0 = GPIO_IN;
    GPCRG1 = GPIO_OUT | GPIO_UP;
    GPCRG2 = GPIO_OUT;
    GPCRG3 = GPIO_ALT;
    GPCRG4 = GPIO_ALT;
    GPCRG5 = GPIO_ALT;
    GPCRG6 = GPIO_OUT | GPIO_UP;
    GPCRG7 = GPIO_ALT;
    GPCRH0 = GPIO_IN;
    GPCRH1 = GPIO_IN;
    GPCRH2 = GPIO_OUT | GPIO_UP;
    GPCRH3 = GPIO_IN;
    GPCRH4 = GPIO_IN;
    GPCRH5 = GPIO_OUT | GPIO_UP;
    GPCRH6 = GPIO_IN;
    GPCRH7 = GPIO_IN;
    GPCRI0 = GPIO_ALT;
    GPCRI1 = GPIO_ALT;
    GPCRI2 = GPIO_ALT;
    GPCRI3 = GPIO_ALT;
    GPCRI4 = GPIO_ALT;
    GPCRI5 = GPIO_ALT;
    GPCRI6 = GPIO_ALT;
    GPCRI7 = GPIO_IN;
    GPCRJ0 = GPIO_OUT | GPIO_UP;
    GPCRJ1 = GPIO_IN;
    GPCRJ2 = GPIO_OUT;
    GPCRJ3 = GPIO_IN;
    GPCRJ4 = GPIO_OUT;
    GPCRJ5 = GPIO_OUT;
    GPCRJ6 = GPIO_OUT | GPIO_UP;
    GPCRJ7 = GPIO_IN;
    GPCRM0 = 0x06;
    GPCRM1 = 0x06;
    GPCRM2 = 0x06;
    GPCRM3 = 0x06;
    GPCRM4 = 0x06;
    GPCRM5 = GPIO_ALT;
    GPCRM6 = 0x86;
    //GPCRM7 = 0x86;
}

#if GPIO_DEBUG
void gpio_debug_bank(
    char * bank,
    uint8_t data,
    uint8_t mirror,
    uint8_t pot,
    volatile uint8_t * control
) {
    for(char i = 0; i < 8; i++) {
        DEBUG(
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
#endif
