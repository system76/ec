// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/macro.h>

// clang-format off
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
struct Gpio __code GC6_FB_EN =      GPIO(G, 1);
struct Gpio __code LAN_WAKEUP_N =   GPIO(B, 2);
struct Gpio __code LED_ACIN =       GPIO(C, 7);
struct Gpio __code LED_AIRPLANE_N = GPIO(G, 6);
struct Gpio __code LED_CAP_N =      GPIO(J, 5);
struct Gpio __code LED_BAT_CHG =    GPIO(H, 3);
struct Gpio __code LED_BAT_FULL =   GPIO(H, 4);
struct Gpio __code LED_NUM_N =      GPIO(J, 4);
struct Gpio __code LED_PWR =        GPIO(H, 5);
struct Gpio __code LED_SCROLL_N =   GPIO(J, 3);
struct Gpio __code LID_SW_N =       GPIO(D, 1);
struct Gpio __code ME_WE =          GPIO(I, 2);
struct Gpio __code PM_CLKRUN_N =    GPIO(H, 0); // renamed to ECCLKRUN#
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
struct Gpio __code VA_EC_EN =       GPIO(J, 0); // renamed to SLP_SUS_EC#
struct Gpio __code WLAN_EN =        GPIO(J, 2);
struct Gpio __code WLAN_PWR_EN =    GPIO(B, 0);
// clang-format on

void gpio_init() {
    // Enable LPC reset on GPD2
    GCR = 0x04;

    // Set GPIO data
    // SYS_FAN
    GPDRA = BIT(3);
    GPDRB = 0x00;
    GPDRC = 0x00;
    // PWR_BTN#, SCI#, SMI#
    GPDRD = BIT(5) | BIT(4) | BIT(3);
    GPDRE = 0x00;
    // H_PECI
    GPDRF = BIT(6);
    // AIRPLAN_LED#
    GPDRG = BIT(6);
    GPDRH = 0x00;
    GPDRI = 0x00;
    // LED_CAP#, LED_NUM#, LED_SCROLL#, KBC_MUTE#
    GPDRJ = BIT(5) | BIT(4) | BIT(3) | BIT(1);

    // Set GPIO control
    // EC_PWM_LEDKB_P
    GPCRA0 = GPIO_ALT;
    // KBC_BEEP
    GPCRA1 = GPIO_ALT;
    // CPU_FAN
    GPCRA2 = GPIO_ALT;
    // SYS_FAN TODO
    GPCRA3 = GPIO_OUT | GPIO_UP;
    // VGA_FAN
    GPCRA4 = GPIO_ALT;
    // EC_PWM_LEDKB_R
    GPCRA5 = GPIO_ALT;
    // EC_PWM_LEDKB_G
    GPCRA6 = GPIO_ALT;
    // EC_PWM_LEDKB_B
    GPCRA7 = GPIO_ALT;
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
    // PERKB-DET#
    GPCRB7 = GPIO_IN | GPIO_UP;
    // ALL_SYS_PWRGD
    GPCRC0 = GPIO_IN;
    // SMC_VGA_THERM
    GPCRC1 = GPIO_OUT;
    // SMD_VGA_THERM
    GPCRC2 = GPIO_OUT;
    // KB-SO16
    GPCRC3 = GPIO_IN | GPIO_UP;
    // CNVI_DET#_EC
    GPCRC4 = GPIO_IN | GPIO_UP;
    // KB-SO17
    GPCRC5 = GPIO_IN | GPIO_UP;
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
    // ALL_FANSEN
    GPCRD7 = GPIO_ALT;
    // SWI#
    GPCRE0 = GPIO_OUT | GPIO_UP;
    // OVERT#
    GPCRE1 = GPIO_IN;
    // RGBKB-DET#
    GPCRE2 = GPIO_IN | GPIO_UP;
    // DGPU_PWR_EN
    GPCRE3 = GPIO_IN;
    // DD_ON
    GPCRE4 = GPIO_OUT | GPIO_DOWN;
    // EC_RSMRST#
    GPCRE5 = GPIO_OUT;
    // SB_KBCRST#
    GPCRE6 = GPIO_OUT | GPIO_UP;
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
    // GC6_FB_EN_PCH
    GPCRG1 = GPIO_IN;
    // AUTO_LOAD
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
    // ECCLKRUN#
    GPCRH0 = GPIO_IN;
    // SUSC#_PCH
    GPCRH1 = GPIO_IN;
    // BKL_EN
    GPCRH2 = GPIO_OUT | GPIO_UP;
    // LED_BAT_CHG
    GPCRH3 = GPIO_OUT | GPIO_UP;
    // LED_BAT_FULL
    GPCRH4 = GPIO_OUT | GPIO_UP;
    // LED_PWR
    GPCRH5 = GPIO_OUT | GPIO_UP;
    // SUSB#_PCH
    GPCRH6 = GPIO_IN;
    // TODO
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
    // MPS_ID
    GPCRI5 = GPIO_IN;
    // FANSEN_SEL (L:VGA H:SYS)
    GPCRI6 = GPIO_OUT;
    // MODEL_ID
    GPCRI7 = GPIO_IN;
    // SLP_SUS_EC#
    GPCRJ0 = GPIO_OUT;
    // KBC_MUTE#
    GPCRJ1 = GPIO_OUT;
    // WLAN_EN
    GPCRJ2 = GPIO_OUT | GPIO_UP;
    // LED_SCROLL#
    GPCRJ3 = GPIO_OUT | GPIO_UP;
    // LED_NUM#
    GPCRJ4 = GPIO_OUT | GPIO_UP;
    // LED_CAP#
    GPCRJ5 = GPIO_OUT | GPIO_UP;
    // POWER_IC_EN
    GPCRJ6 = GPIO_OUT | GPIO_UP;
    // VBATT_BOOST#
    GPCRJ7 = GPIO_OUT;
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
