// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/macro.h>

// uncrustify:off
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
struct Gpio __code RGBKB_DET_N =    GPIO(E, 2);
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
// uncrustify:on

static const struct GpioInit __code gpio_cfg_init[] = {
    // General control
    { &GCR, 0x04 }, // Enable LPC reset on GPD2

    // Port data
    { &GPDRA, BIT(3) }, // SYS_FAN
    { &GPDRB, 0 },
    { &GPDRC, 0 },
    { &GPDRD, BIT(5) | BIT(4) | BIT(3) }, // PWR_BTN#, SCI#, SMI#
    { &GPDRE, 0 },
    { &GPDRF, BIT(6) }, // H_PECI
    { &GPDRG, BIT(6) }, // AIRPLAN_LED#
    { &GPDRH, 0 },
    { &GPDRI, 0 },
    { &GPDRJ, BIT(5) | BIT(4) | BIT(3) | BIT(1) }, // LED_CAP#, LED_NUM#, LED_SCROLL#, KBC_MUTE#

    // Port control
    { &GPCRA0, GPIO_ALT }, // EC_PWM_LEDKB_P
    { &GPCRA1, GPIO_ALT }, // KBC_BEEP
    { &GPCRA2, GPIO_ALT }, // CPU_FAN
    { &GPCRA3, GPIO_OUT | GPIO_UP }, // TODO: SYS_FAN
    { &GPCRA4, GPIO_ALT }, // VGA_FAN
    { &GPCRA5, GPIO_ALT }, // EC_PWM_LEDKB_R
    { &GPCRA6, GPIO_ALT }, // EC_PWM_LEDKB_G
    { &GPCRA7, GPIO_ALT }, // EC_PWM_LEDKB_B

    { &GPCRB0, GPIO_OUT | GPIO_UP }, // WLAN_PWR_EN
    { &GPCRB1, GPIO_OUT | GPIO_UP }, // H_PROCHOT_EC
    { &GPCRB2, GPIO_IN | GPIO_UP }, // LAN_WAKEUP#
    { &GPCRB3, GPIO_ALT }, // SMC_BAT
    { &GPCRB4, GPIO_ALT }, // SMD_BAT
    { &GPCRB5, GPIO_OUT | GPIO_UP }, // SUSBC_EN#
    { &GPCRB6, GPIO_IN | GPIO_UP }, // AC_IN#
    { &GPCRB7, GPIO_IN | GPIO_UP }, // PERKB-DET#

    { &GPCRC0, GPIO_IN }, // ALL_SYS_PWRGD
    { &GPCRC1, GPIO_OUT }, // SMC_VGA_THERM
    { &GPCRC2, GPIO_OUT }, // SMD_VGA_THERM
    { &GPCRC3, GPIO_ALT | GPIO_UP }, // KB-SO16
    { &GPCRC4, GPIO_IN | GPIO_UP }, // CNVI_DET#_EC
    { &GPCRC5, GPIO_ALT | GPIO_UP }, // KB-SO17
    { &GPCRC6, GPIO_OUT }, // PM_PWROK
    { &GPCRC7, GPIO_OUT | GPIO_UP }, // LED_ACIN

    { &GPCRD0, GPIO_IN | GPIO_UP }, // PWR_SW#
    { &GPCRD1, GPIO_IN | GPIO_UP }, // LID_SW#
    { &GPCRD2, GPIO_ALT }, // BUF_PLT_RST#
    { &GPCRD3, GPIO_IN }, // SMI#
    { &GPCRD4, GPIO_IN }, // SCI#
    { &GPCRD5, GPIO_OUT | GPIO_UP }, // PWR_BTN#
    { &GPCRD6, GPIO_ALT }, // CPU_FANSEN
    { &GPCRD7, GPIO_ALT }, // ALL_FANSEN

    { &GPCRE0, GPIO_OUT | GPIO_UP }, // SWI#
    { &GPCRE1, GPIO_IN }, // OVERT#
    { &GPCRE2, GPIO_IN | GPIO_UP }, // RGBKB-DET#
    { &GPCRE3, GPIO_IN }, // DGPU_PWR_EN
    { &GPCRE4, GPIO_OUT | GPIO_DOWN }, // DD_ON
    { &GPCRE5, GPIO_OUT }, // EC_RSMRST#
    { &GPCRE6, GPIO_OUT | GPIO_UP }, // SB_KBCRST#
    { &GPCRE7, GPIO_OUT | GPIO_UP }, // AC_PRESENT

    { &GPCRF0, GPIO_IN }, // 80CLK
    { &GPCRF1, GPIO_OUT | GPIO_UP }, // USB_CHARGE_EN
    { &GPCRF2, GPIO_IN | GPIO_UP }, // 3IN1
    { &GPCRF3, GPIO_OUT | GPIO_UP }, // BT_EN
    { &GPCRF4, GPIO_ALT }, // TP_CLK
    { &GPCRF5, GPIO_ALT }, // TP_DATA
    { &GPCRF6, GPIO_ALT }, // H_PECI
    { &GPCRF7, GPIO_OUT | GPIO_UP }, // USB_PWR_EN#

    { &GPCRG0, GPIO_OUT | GPIO_UP }, // CCD_EN
    { &GPCRG1, GPIO_IN }, // GC6_FB_EN_PCH
    { &GPCRG2, GPIO_OUT }, // AUTO_LOAD
    { &GPCRG3, GPIO_ALT }, // ALSPI_CE#
    { &GPCRG4, GPIO_ALT }, // ALSPI_MSI
    { &GPCRG5, GPIO_ALT }, // ALSPI_MSO
    { &GPCRG6, GPIO_OUT | GPIO_UP }, // AIRPLAN_LED#
    { &GPCRG7, GPIO_ALT }, // ALSPI_SCLK

    { &GPCRH0, GPIO_IN }, // ECCLKRUN#
    { &GPCRH1, GPIO_IN }, // SUSC#_PCH
    { &GPCRH2, GPIO_OUT | GPIO_UP }, // BKL_EN
    { &GPCRH3, GPIO_OUT | GPIO_UP }, // LED_BAT_CHG
    { &GPCRH4, GPIO_OUT | GPIO_UP }, // LED_BAT_FULL
    { &GPCRH5, GPIO_OUT | GPIO_UP }, // LED_PWR
    { &GPCRH6, GPIO_IN }, // SUSB#_PCH

    { &GPCRI0, GPIO_ALT }, // BAT_DET
    { &GPCRI1, GPIO_ALT }, // BAT_VOLT
    { &GPCRI2, GPIO_OUT }, // ME_WE
    { &GPCRI3, GPIO_ALT }, // THERM_VOLT
    { &GPCRI4, GPIO_ALT }, // TOTAL_CUR
    { &GPCRI5, GPIO_IN }, // MPS_ID
    { &GPCRI6, GPIO_OUT }, // FANSEN_SEL (L:VGA H:SYS)
    { &GPCRI7, GPIO_IN }, // MODEL_ID

    { &GPCRJ0, GPIO_OUT }, // SLP_SUS_EC#
    { &GPCRJ1, GPIO_OUT }, // KBC_MUTE#
    { &GPCRJ2, GPIO_OUT | GPIO_UP }, // WLAN_EN
    { &GPCRJ3, GPIO_OUT | GPIO_UP }, // LED_SCROLL#
    { &GPCRJ4, GPIO_OUT | GPIO_UP }, // LED_NUM#
    { &GPCRJ5, GPIO_OUT | GPIO_UP }, // LED_CAP#
    { &GPCRJ6, GPIO_OUT | GPIO_UP }, // POWER_IC_EN
    { &GPCRJ7, GPIO_OUT }, // VBATT_BOOST#

    { &GPCRM0, GPIO_ALT }, // LPC_AD0
    { &GPCRM1, GPIO_ALT }, // LPC_AD1
    { &GPCRM2, GPIO_ALT }, // LPC_AD2
    { &GPCRM3, GPIO_ALT }, // LPC_AD3
    { &GPCRM4, GPIO_ALT }, // PCLK_KBC
    { &GPCRM5, GPIO_ALT }, // LPC_FRAME#
    { &GPCRM6, GPIO_ALT }, // SERIRQ
};

void gpio_init(void) {
    for (uint8_t i = 0; i < ARRAY_SIZE(gpio_cfg_init); i++) {
        *gpio_cfg_init[i].reg = gpio_cfg_init[i].data;
    }
}
