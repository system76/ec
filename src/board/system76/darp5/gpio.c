// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

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
struct Gpio __code EC_EN =          GPIO(E, 1);
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code LAN_WAKEUP_N =   GPIO(B, 2);
struct Gpio __code LED_ACIN =       GPIO(C, 7);
struct Gpio __code LED_AIRPLANE_N = GPIO(G, 6);
struct Gpio __code LED_BAT_CHG =    GPIO(A, 5);
struct Gpio __code LED_BAT_FULL =   GPIO(A, 6);
struct Gpio __code LED_PWR =        GPIO(A, 7);
struct Gpio __code LID_SW_N =       GPIO(D, 1);
struct Gpio __code ME_WE =          GPIO(J, 2);
struct Gpio __code PCH_DPWROK_EC =  GPIO(A, 3);
struct Gpio __code PCH_PWROK_EC =   GPIO(A, 4);
struct Gpio __code PM_CLKRUN_N =    GPIO(H, 0);
struct Gpio __code PM_PWROK =       GPIO(C, 6);
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(D, 0);
struct Gpio __code RGBKB_DET_N =    GPIO(H, 3);
struct Gpio __code SB_KBCRST_N =    GPIO(E, 6);
struct Gpio __code SCI_N =          GPIO(D, 4);
struct Gpio __code SLP_SUS_N =      GPIO(I, 2);
struct Gpio __code SMI_N =          GPIO(D, 3);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code SUSWARN_N =      GPIO(D, 7);
struct Gpio __code SUS_PWR_ACK =    GPIO(J, 0);
struct Gpio __code SWI_N =          GPIO(E, 0);
struct Gpio __code USB_PWR_EN_N =   GPIO(F, 7);
struct Gpio __code VA_EC_EN =       GPIO(E, 3);
struct Gpio __code VR_ON =          GPIO(H, 4);
struct Gpio __code WLAN_EN =        GPIO(H, 5);
struct Gpio __code WLAN_PWR_EN =    GPIO(J, 4);
// uncrustify:on

static const struct GpioInit __code gpio_cfg_init[] = {
    // General control
    { &GCR, 0x04 }, // Enable LPC reset on GPD2

    // Port data
    { &GPDRA, 0 },
    { &GPDRB, BIT(0) }, // NC
    { &GPDRC, 0 },
    { &GPDRD, BIT(5) | BIT(4) | BIT(3) }, // PWR_BTN#, SCI#, SMI#
    { &GPDRE, 0 },
    { &GPDRF, BIT(7) | BIT(6) }, // USB_PWR_EN#, H_PECI
    { &GPDRG, BIT(6) }, // AIRPLAN_LED#
    { &GPDRH, 0 },
    { &GPDRI, 0 },
    { &GPDRJ, 0 },

    // Port control
    { &GPCRA0, GPIO_IN }, // EC_SSD_LED#
    { &GPCRA1, GPIO_ALT }, // KBC_BEEP
    { &GPCRA2, GPIO_ALT }, // CPU_FAN
    { &GPCRA3, GPIO_IN }, // PCH_DPWROK_EC
    { &GPCRA4, GPIO_OUT }, // PCH_PWROK_EC
    { &GPCRA5, GPIO_OUT }, // LED_BAT_CHG
    { &GPCRA6, GPIO_OUT }, // LED_BAT_FULL
    { &GPCRA7, GPIO_OUT }, // LED_PWR

    { &GPCRB0, GPIO_OUT }, // NC
    { &GPCRB1, GPIO_OUT }, // H_PROCHOT_EC
    { &GPCRB2, GPIO_IN | GPIO_UP }, // LAN_WAKEUP#
    { &GPCRB3, GPIO_ALT }, // SMC_BAT
    { &GPCRB4, GPIO_ALT }, // SMD_BAT
    { &GPCRB5, GPIO_OUT }, // GA20
    { &GPCRB6, GPIO_IN | GPIO_UP }, // AC_IN#
    { &GPCRB7, GPIO_IN }, // FP_RST#

    { &GPCRC0, GPIO_IN }, // ALL_SYS_PWRGD
    { &GPCRC1, GPIO_ALT }, // SMC_VGA_THERM
    { &GPCRC2, GPIO_ALT }, // SMD_VGA_THERM
    { &GPCRC3, GPIO_ALT | GPIO_UP }, // KSO16 (Darter)
    { &GPCRC4, GPIO_OUT }, // CNVI_DET#
    { &GPCRC5, GPIO_ALT | GPIO_UP }, // KSO17 (Darter)
    { &GPCRC6, GPIO_OUT }, // PM_PWROK
    { &GPCRC7, GPIO_OUT }, // LED_ACIN

    { &GPCRD0, GPIO_IN | GPIO_UP }, // PWR_SW#
    { &GPCRD1, GPIO_IN | GPIO_UP }, // LID_SW#
    { &GPCRD2, GPIO_ALT }, // BUF_PLT_RST#
    { &GPCRD3, GPIO_IN }, // SMI#
    { &GPCRD4, GPIO_IN }, // SCI#
    { &GPCRD5, GPIO_OUT }, // PWR_BTN#
    { &GPCRD6, GPIO_ALT }, // CPU_FANSEN
    { &GPCRD7, GPIO_IN }, // SUSWARN#

    { &GPCRE0, GPIO_OUT }, // SWI#
    { &GPCRE1, GPIO_OUT }, // EC_EN
    { &GPCRE2, GPIO_IN }, // PCH_SLP_WLAN#_R
    { &GPCRE3, GPIO_OUT }, // VA_EC_EN
    { &GPCRE4, GPIO_OUT }, // DD_ON
    { &GPCRE5, GPIO_OUT }, // EC_RSMRST#
    { &GPCRE6, GPIO_OUT }, // SB_KBCRST#
    { &GPCRE7, GPIO_OUT }, // AC_PRESENT / PM_PWROK

    { &GPCRF0, GPIO_IN }, // 80CLK
    { &GPCRF1, GPIO_OUT }, // USB_CHARGE_EN
    { &GPCRF2, GPIO_IN | GPIO_UP }, // 3IN1
    { &GPCRF3, GPIO_OUT }, // BT_EN
    { &GPCRF4, GPIO_ALT }, // TP_CLK
    { &GPCRF5, GPIO_ALT }, // TP_DATA
    { &GPCRF6, GPIO_ALT }, // H_PECI
    { &GPCRF7, GPIO_OUT }, // USB_PWR_EN#

    { &GPCRG0, GPIO_OUT }, // CCD_EN
    { &GPCRG1, GPIO_OUT }, // 3G_EN
    { &GPCRG2, GPIO_OUT }, // VDD3
    { &GPCRG3, GPIO_ALT }, // HSPI_CE#
    { &GPCRG4, GPIO_ALT }, // HSPI_MSI
    { &GPCRG5, GPIO_ALT }, // HSPI_MSO
    { &GPCRG6, GPIO_OUT }, // AIRPLAN_LED#
    { &GPCRG7, GPIO_ALT }, // HCPI_SCLK

    { &GPCRH0, GPIO_ALT }, // EC_CLKRUN#
    { &GPCRH1, GPIO_IN }, // SUSC#_PCH
    { &GPCRH2, GPIO_OUT }, // BKL_EN
    { &GPCRH3, GPIO_IN | GPIO_UP }, // RGBKB-DET#
    { &GPCRH4, GPIO_IN }, // VR_ON
    { &GPCRH5, GPIO_OUT }, // WLAN_EN
    { &GPCRH6, GPIO_IN }, // SUSB#_PCH

    { &GPCRI0, GPIO_ALT }, // BAT_DET
    { &GPCRI1, GPIO_ALT }, // BAT_VOLT
    { &GPCRI2, GPIO_IN }, // SLP_SUS#
    { &GPCRI3, GPIO_ALT }, // THERM_VOLT
    { &GPCRI4, GPIO_ALT }, // TOTAL_CUR
    { &GPCRI5, GPIO_IN }, // AZ_RST#_EC
    { &GPCRI6, GPIO_IN }, // LIGHT_KB_DET#
    { &GPCRI7, GPIO_IN }, // MODEL_ID

    { &GPCRJ0, GPIO_IN | GPIO_DOWN }, // SUS_PWR_ACK
    { &GPCRJ1, GPIO_IN }, // KBC_MUTE#
    { &GPCRJ2, GPIO_OUT }, // ME_WE
    { &GPCRJ3, GPIO_IN }, // SOC_TYPE
    { &GPCRJ4, GPIO_OUT }, // WLAN_PWR_EN
    { &GPCRJ5, GPIO_ALT }, // KBLIGHT_ADJ
    { &GPCRJ6, GPIO_OUT }, // 3G_PWR_EN
    { &GPCRJ7, GPIO_IN }, // NC

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
