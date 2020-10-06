// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/debug.h>

struct Gpio __code ACIN_N =         GPIO(B, 6);
struct Gpio __code AC_PRESENT =     GPIO(E, 7);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(H, 2);
struct Gpio __code BT_EN =          GPIO(F, 3);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2);
struct Gpio __code CCD_EN =         GPIO(G, 0);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code DGPU_PWR_EN =    GPIO(E, 3);
struct Gpio __code EC_EN =          GPIO(B, 5); // renamed to SUSBC_EN
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code GC6_FB_EN =      GPIO(J, 4);
struct Gpio __code LAN_WAKEUP_N =   GPIO(B, 2);
struct Gpio __code LED_ACIN =       GPIO(C, 7);
struct Gpio __code LED_AIRPLANE_N = GPIO(G, 6);
struct Gpio __code LED_BAT_CHG =    GPIO(A, 5);
struct Gpio __code LED_BAT_FULL =   GPIO(A, 6);
struct Gpio __code LED_PWR =        GPIO(A, 7);
struct Gpio __code LID_SW_N =       GPIO(D, 1);
struct Gpio __code PM_PWROK =       GPIO(C, 6);
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(D, 0);
struct Gpio __code SB_KBCRST_N =    GPIO(E, 6);
struct Gpio __code SCI_N =          GPIO(D, 4);
struct Gpio __code SMI_N =          GPIO(D, 3);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code SWI_N =          GPIO(E, 0);
struct Gpio __code USB_PWR_EN_N =   GPIO(F, 7);
struct Gpio __code VA_EC_EN =       GPIO(J, 0); // renamed to EC_SLP_SUS#
struct Gpio __code WLAN_EN =        GPIO(J, 7);
struct Gpio __code WLAN_PWR_EN =    GPIO(B, 0);

void gpio_init(void) {
    // Enable LPC reset on GPD2
    GCR = 0x04;

    // Set GPIO data
    GPDRA = 0x00;
    GPDRB = 0x00;
    GPDRC = 0x00;
    // PWR_BTN#, SCI#, SMI#
    GPDRD = (1U << 5) | (1U << 4) | (1U << 3);
    // AMP_EN
    GPDRE = (1U << 1);
    // USB_PWR_EN#, H_PECI
    GPDRF = (1U << 7) | (1U << 6);
    // AIRPLAN_LED#
    GPDRG = (1U << 6);
    GPDRH = 0x00;
    // EC_AMP_EN
    GPDRI = (1U << 5);
    GPDRJ = 0x00;

    // EC_SSD_LED#
    GPCRA0 = GPIO_IN;
    // KBC_BEEP
    GPCRA1 = GPIO_ALT;
    // CPU_FAN_PWM
    GPCRA2 = GPIO_ALT;
    // VGA_FAN_PWM1
    GPCRA3 = GPIO_ALT;
    // VGA_FAN_PWM2
    GPCRA4 = GPIO_ALT;
    // LED_BAT_CHG
    GPCRA5 = GPIO_OUT | GPIO_UP;
    // LED_BAT_FULL
    GPCRA6 = GPIO_OUT | GPIO_UP;
    // LED_PWR
    GPCRA7 = GPIO_OUT | GPIO_UP;

    // WLAN_PWR_EN
    GPCRB0 = GPIO_OUT | GPIO_UP;
    // H_PROCHOT_EC
    GPCRB1 = GPIO_OUT | GPIO_UP;
    // LAN_WAKEUP#
    GPCRB2 = GPIO_IN | GPIO_UP;
    // SMC_BAT
    GPCRB3 = GPIO_ALT;
    // SMD_BAT
    GPCRB4 = GPIO_ALT;
    // SUSBC_EN#
    GPCRB5 = GPIO_OUT | GPIO_UP;
    // AC_IN#
    GPCRB6 = GPIO_IN | GPIO_UP;
    // VBATT_BOOST#
    GPCRB7 = GPIO_OUT;

    // ALL_SYS_PWRGD
    GPCRC0 = GPIO_IN;
    // SMC_VGA_THERM
    GPCRC1 = GPIO_ALT;
    // SMD_VGA_THERM
    GPCRC2 = GPIO_ALT;
    // KB-SO16
    GPCRC3 = GPIO_IN;
    // CNVI_WIGIG_DET#
    GPCRC4 = GPIO_IN | GPIO_UP;
    // KB-SO17
    GPCRC5 = GPIO_IN;
    // PM_PWROK
    GPCRC6 = GPIO_OUT;
    // LED_ACIN
    GPCRC7 = GPIO_OUT | GPIO_UP;

    // PWR_SW#
    GPCRD0 = GPIO_IN | GPIO_UP;
    // LID_SW#
    GPCRD1 = GPIO_IN | GPIO_UP;
    // BUF_PLT_RST#
    GPCRD2 = GPIO_ALT;
    // SMI#
    GPCRD3 = GPIO_IN;
    // SCI#
    GPCRD4 = GPIO_IN;
    // PWR_BTN#
    GPCRD5 = GPIO_OUT | GPIO_UP;
    // CPU_FANSEN
    GPCRD6 = GPIO_ALT;
    // VGA_FANSEN
    GPCRD7 = GPIO_ALT;

    // SWI#
    GPCRE0 = GPIO_OUT | GPIO_UP;
    // AMP_EN
    GPCRE1 = GPIO_OUT | GPIO_UP;
    // PERKB_DET#
    GPCRE2 = GPIO_IN | GPIO_UP;
    // DGPU_PWR_EN
    GPCRE3 = GPIO_IN;
    // DD_ON
    GPCRE4 = GPIO_OUT | GPIO_DOWN;
    // EC_RSMRST#
    GPCRE5 = GPIO_OUT;
    // SB_KBCRST#
    GPCRE6 = GPIO_IN;
    // AC_PRESENT
    GPCRE7 = GPIO_OUT | GPIO_UP;

    // 80CLK
    GPCRF0 = GPIO_IN;
    // USB_CHARGE_EN
    GPCRF1 = GPIO_OUT | GPIO_UP;
    // 3IN1
    GPCRF2 = GPIO_IN | GPIO_UP;
    // BT_EN
    GPCRF3 = GPIO_OUT | GPIO_UP;
    // TP_CLK
    GPCRF4 = GPIO_ALT;
    // TP_DATA
    GPCRF5 = GPIO_ALT;
    // H_PECI
    GPCRF6 = GPIO_ALT;
    // USB_PWR_EN#
    GPCRF7 = GPIO_OUT | GPIO_UP;

    // CCD_EN
    GPCRG0 = GPIO_OUT | GPIO_UP;
    // 3G_EN
    GPCRG1 = GPIO_OUT | GPIO_UP;
    // AUTO_LOAD_PWR
    GPCRG2 = GPIO_OUT;
    // ALSPI_CE#
    GPCRG3 = GPIO_ALT;
    // ALSPI_MSI
    GPCRG4 = GPIO_ALT;
    // ALSPI_MSO
    GPCRG5 = GPIO_ALT;
    // AIRPLAN_LED#
    GPCRG6 = GPIO_OUT | GPIO_UP;
    // ALSPI_SCLK
    GPCRG7 = GPIO_ALT;

    // NV_POWER_IC_EN
    GPCRH0 = GPIO_OUT | GPIO_UP;
    // SUSC#_PCH
    GPCRH1 = GPIO_IN;
    // BKL_EN
    GPCRH2 = GPIO_OUT | GPIO_UP;
    // dGPU_GPIO8_OVERT
    GPCRH3 = GPIO_IN | GPIO_UP;
    // d_GPIO9_ALERT_FAN / 3G_RST#
    GPCRH4 = GPIO_IN | GPIO_UP;
    // WL_KB_DET#
    GPCRH5 = GPIO_IN | GPIO_UP;
    // SUSB#_PCH
    GPCRH6 = GPIO_IN;
    // Unknown
    GPCRH7 = GPIO_IN;

    // BAT_DET
    GPCRI0 = GPIO_ALT;
    // BAT_VOLT
    GPCRI1 = GPIO_ALT;
    // ME_WE
    GPCRI2 = GPIO_OUT;
    // THERM_VOLT
    GPCRI3 = GPIO_ALT;
    // TOTAL_CUR
    GPCRI4 = GPIO_ALT;
    // EC_AMP_EN
    GPCRI5 = GPIO_OUT;
    // VGASEN_SEL
    GPCRI6 = GPIO_OUT;
    // MODEL_ID
    GPCRI7 = GPIO_IN;

    // EC_SLP_SUS#
    GPCRJ0 = GPIO_OUT;
    // KBC_MUTE# / PERKB_ID1#
    GPCRJ1 = GPIO_IN;
    // KBLIGHT_ADJ
    GPCRJ2 = GPIO_ALT;
    // RGBKB_DET#
    GPCRJ3 = GPIO_IN | GPIO_UP;
    // GC6_FB_EN_PCH
    GPCRJ4 = GPIO_IN;
    // MCU_GPE2 / PERKB_ID2#
    GPCRJ5 = GPIO_IN;
    // 3G_PWR_EN
    GPCRJ6 = GPIO_OUT | GPIO_UP;
    // WLAN_EN
    GPCRJ7 = GPIO_OUT | GPIO_UP;

    // LPC_AD0
    GPCRM0 = GPIO_ALT;
    // LPC_AD1
    GPCRM1 = GPIO_ALT;
    // LPC_AD2
    GPCRM2 = GPIO_ALT;
    // LPC_AD3
    GPCRM3 = GPIO_ALT;
    // PCLK_KBC
    GPCRM4 = GPIO_ALT;
    // LPC_FRAME#
    GPCRM5 = GPIO_ALT;
    // SERIRQ
    GPCRM6 = GPIO_ALT;
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
