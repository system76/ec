// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/macro.h>

// uncrustify:off
struct Gpio __code ACIN_N =         GPIO(B, 0);
struct Gpio __code AC_PRESENT =     GPIO(E, 1);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(H, 2);
struct Gpio __code BT_EN =          GPIO(F, 3);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2);
struct Gpio __code CCD_EN =         GPIO(D, 1);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code DGPU_PWR_EN =    GPIO(J, 2);
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code GC6_FB_EN =      GPIO(J, 3);
struct Gpio __code LAN_WAKEUP_N =   GPIO(B, 2);
struct Gpio __code LED_ACIN =       GPIO(C, 7);
struct Gpio __code LED_AIRPLANE_N = GPIO(H, 7);
struct Gpio __code LED_BAT_CHG =    GPIO(H, 5);
struct Gpio __code LED_BAT_FULL =   GPIO(J, 0);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code ME_WE =          GPIO(I, 2);
struct Gpio __code PM_PWROK =       GPIO(C, 6);
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code RGBKB_DET_N =    GPIO(E, 2);
struct Gpio __code SCI_N =          GPIO(D, 3);
struct Gpio __code SMI_N =          GPIO(D, 4);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code SUSWARN_N =      GPIO(H, 0);
struct Gpio __code SUS_PWR_ACK =    GPIO(F, 7);
struct Gpio __code SWI_N =          GPIO(B, 5);
struct Gpio __code VA_EC_EN =       GPIO(J, 4); // renamed to SLP_SUS#
struct Gpio __code WLAN_EN =        GPIO(G, 1);
struct Gpio __code WLAN_PWR_EN =    GPIO(A, 3);
struct Gpio __code XLP_OUT =        GPIO(B, 4);
// uncrustify:on

static const struct GpioInit __code gpio_cfg_init[] = {
    // General control
    { &GCR9, BIT(5) }, // PWRSW WDT 2 Enable 2
    { &GCR8, BIT(4) }, // PWRSW WDT 2 Enable 1
    { &GCR, 0x04 }, // Enable LPC reset on GPD2
    { &GCR15, BIT(4) }, // Enable SMBus channel 4
    { &GCR20, 0 }, // Set GPF2 and GPF3 to 3.3V

    // Port data
    { &GPDRA, 0 },
    { &GPDRB, BIT(6) | BIT(4) | BIT(3) }, // H_PROCHOT_EC, XLP_OUT, PWR_SW#
    { &GPDRC, 0 },
    { &GPDRD, BIT(5) | BIT(4) | BIT(3) }, // PWR_BTN#, SMI#, SCI#
    { &GPDRE, 0 },
    { &GPDRF, BIT(6) }, // EC_PECI
    { &GPDRG, 0 },
    { &GPDRH, BIT(7) }, // AIRPLAN_LED#
    { &GPDRI, 0 },
    { &GPDRJ, 0 },

    // Port control
    { &GPCRA0, GPIO_ALT }, // EC_PWM_PIN_24
    { &GPCRA1, GPIO_ALT }, // KBC_BEEP
    { &GPCRA2, GPIO_ALT }, // CPU_FAN
    { &GPCRA3, GPIO_OUT | GPIO_UP }, // WLAN_PWR_EN
    { &GPCRA4, GPIO_ALT }, // VGA_FAN
    { &GPCRA5, GPIO_ALT }, // EC_PWM_LEDKB_R
    { &GPCRA6, GPIO_ALT }, // EC_PWM_LEDKB_G
    { &GPCRA7, GPIO_ALT }, // EC_PWM_LEDKB_B

    { &GPCRB0, GPIO_IN | GPIO_UP }, // AC_IN#
    { &GPCRB1, GPIO_IN | GPIO_UP }, // LID_SW#
    { &GPCRB2, GPIO_IN | GPIO_UP }, // LAN_WAKEUP#
    { &GPCRB3, GPIO_IN }, // PWR_SW#
    { &GPCRB4, GPIO_OUT }, // XLP_OUT
    { &GPCRB5, GPIO_OUT | GPIO_UP }, // SWI#
    { &GPCRB6, GPIO_OUT | GPIO_UP }, // H_PROCHOT_EC

    { &GPCRC0, GPIO_IN }, // ALL_SYS_PWRGD
    { &GPCRC1, GPIO_ALT }, // SMC_VGA_THERM
    { &GPCRC2, GPIO_ALT }, // SMD_VGA_THERM
    { &GPCRC3, GPIO_ALT | GPIO_UP }, // KB-SO16
    { &GPCRC4, GPIO_IN | GPIO_UP }, // CNVI_DET#_EC
    { &GPCRC5, GPIO_ALT | GPIO_UP }, // KB-SO17
    { &GPCRC6, GPIO_OUT }, // PM_PWROK
    { &GPCRC7, GPIO_OUT | GPIO_UP }, // LED_ACIN

    { &GPCRD0, GPIO_OUT | GPIO_UP }, // LED_PWR
    { &GPCRD1, GPIO_OUT | GPIO_UP }, // CCD_EN
    { &GPCRD2, GPIO_ALT }, // BUF_PLT_RST#
    { &GPCRD3, GPIO_IN }, // SCI#
    { &GPCRD4, GPIO_IN }, // SMI#
    { &GPCRD5, GPIO_OUT | GPIO_UP }, // PWR_BTN#
    { &GPCRD6, GPIO_ALT }, // CPU_FANSEN
    { &GPCRD7, GPIO_ALT }, // VGA_FANSEN

    { &GPCRE0, GPIO_ALT }, // SMC_BAT
    { &GPCRE1, GPIO_OUT | GPIO_UP }, // AC_PRESENT
    { &GPCRE2, GPIO_IN | GPIO_UP }, // RGBKB-DET#
    { &GPCRE3, GPIO_OUT }, // USB_PWR_EN# (on 1650/1650Ti), NC (on 1660Ti)
    { &GPCRE4, GPIO_OUT | GPIO_DOWN }, // DD_ON
    { &GPCRE5, GPIO_OUT }, // EC_RSMRST#
    { &GPCRE6, GPIO_IN }, // SB_KBCRST#
    { &GPCRE7, GPIO_ALT }, // SMD_BAT

    { &GPCRF0, GPIO_IN }, // 80CLK
    { &GPCRF1, GPIO_OUT | GPIO_UP }, // USB_CHARGE_EN
    { &GPCRF2, GPIO_IN | GPIO_UP }, // 3IN1
    { &GPCRF3, GPIO_OUT | GPIO_UP }, // BT_EN
    { &GPCRF4, GPIO_ALT }, // TP_CLK
    { &GPCRF5, GPIO_ALT }, // TP_DATA
    { &GPCRF6, GPIO_ALT }, // EC_PECI
    { &GPCRF7, GPIO_IN }, // SUS_PWR_ACK#

    { &GPCRG0, GPIO_IN | GPIO_UP }, // dGPU_GPIO8_OVERT
    { &GPCRG1, GPIO_OUT | GPIO_UP }, // WLAN_EN
    { &GPCRG2, GPIO_OUT }, // AUTO_LOAD_PWR
    { &GPCRG3, GPIO_ALT }, // ALSPI_CE#
    { &GPCRG4, GPIO_ALT }, // ALSPI_MSI
    { &GPCRG5, GPIO_ALT }, // ALSPI_MSO
    { &GPCRG6, GPIO_OUT | GPIO_UP }, // H_PROCHOT#_EC
    { &GPCRG7, GPIO_ALT }, // ALSPI_SCLK

    { &GPCRH0, GPIO_IN }, // SUS_WARN#
    { &GPCRH1, GPIO_IN }, // SUSC#
    { &GPCRH2, GPIO_OUT | GPIO_UP }, // BKL_EN
    { &GPCRH3, GPIO_IN }, // LIGHT_KB_DET#
    { &GPCRH4, GPIO_IN | GPIO_UP }, // d_GPIO9_ALERT_FAN
    { &GPCRH5, GPIO_OUT | GPIO_UP }, // LED_BAT_CHG
    { &GPCRH6, GPIO_IN }, // SUSB#
    { &GPCRH7, GPIO_OUT | GPIO_UP }, // AIRPLAN_LED#

    { &GPCRI0, GPIO_ALT }, // BAT_DET
    { &GPCRI1, GPIO_ALT }, // BAT_VOLT
    { &GPCRI2, GPIO_OUT }, // ME_WE
    { &GPCRI3, GPIO_ALT }, // THERM_VOLT
    { &GPCRI4, GPIO_ALT }, // TOTAL_CUR
    { &GPCRI5, GPIO_IN }, // PERKB_ID#_R
    { &GPCRI6, GPIO_IN }, // PERKB_ID2#_R
    { &GPCRI7, GPIO_IN }, // MODEL_ID

    { &GPCRJ0, GPIO_OUT | GPIO_UP }, // LED_BAT_FULL
    { &GPCRJ1, GPIO_IN }, // KBC_MUTE#
    { &GPCRJ2, GPIO_IN }, // DGPU_PWR_EN
    { &GPCRJ3, GPIO_IN }, // GC6_FB_EN_PCH
    { &GPCRJ4, GPIO_OUT }, // SLP_SUS#
    { &GPCRJ5, GPIO_OUT }, // VBATT_BOOST#
    { &GPCRJ6, GPIO_OUT }, // EC_GPIO
    { &GPCRJ7, GPIO_IN | GPIO_UP }, // PERKB-DET#_R

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
