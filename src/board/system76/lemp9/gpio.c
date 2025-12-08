// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2019 System76, Inc.

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
struct Gpio __code EC_EN =          GPIO(J, 6);
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code EC_SMD_EN_N =    GPIO(I, 6);
struct Gpio __code LED_ACIN =       GPIO(C, 7);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code ME_WE =          GPIO(I, 2);
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
// uncrustify:on

static const struct GpioInit __code gpio_cfg_init[] = {
    // General control
    { &GCR, 0x04 }, // Enable LPC reset on GPD2
    { &GCR15, BIT(4) }, // Enable SMBus channel 4
    { &GCR20, 0 }, // Set GPF2 and GPF3 to 3.3V

    // Port data
    { &GPDRA, 0 },
    { &GPDRB, BIT(4) | BIT(3) },
    { &GPDRC, 0 },
    { &GPDRD, BIT(5) | BIT(4) | BIT(3) },
    { &GPDRE, BIT(3) },
    { &GPDRF, BIT(6) },
    { &GPDRG, 0 },
    { &GPDRH, 0 },
    { &GPDRI, 0 },
    { &GPDRJ, 0 },

    // Port control
    { &GPCRA0, GPIO_IN }, // AC/BATL#
    { &GPCRA1, GPIO_ALT }, // KBC_BEEP
    { &GPCRA2, GPIO_ALT }, // CPU_FAN
    { &GPCRA3, GPIO_OUT }, // WLAN_PWR_EN
    { &GPCRA4, GPIO_IN }, // NC
    { &GPCRA5, GPIO_IN }, // NC
    { &GPCRA6, GPIO_OUT }, // PCH_PWROK_EC
    { &GPCRA7, GPIO_OUT }, // PCH_DPWROK_EC

    { &GPCRB0, GPIO_IN | GPIO_UP }, // AC_IN#
    { &GPCRB1, GPIO_IN | GPIO_UP }, // LID_SW#
    { &GPCRB2, GPIO_IN | GPIO_UP }, // PCH_SLP_WLAN#_R
    { &GPCRB3, GPIO_IN }, // PWR_SW#
    { &GPCRB4, GPIO_OUT }, // XLP_OUT
    { &GPCRB5, GPIO_OUT }, // SWI#
    { &GPCRB6, GPIO_IN }, // NC

    { &GPCRC0, GPIO_IN }, // ALL_SYS_PWRGD
    { &GPCRC1, GPIO_ALT }, // SMB_CLK_EC
    { &GPCRC2, GPIO_ALT }, // SMB_DATA_EC
    { &GPCRC3, GPIO_IN }, // PCIE_WAKE#
    { &GPCRC4, GPIO_IN | GPIO_UP }, // CNVI_DET#
    { &GPCRC5, GPIO_IN }, // NC
    { &GPCRC6, GPIO_OUT }, // PM_PWROK
    { &GPCRC7, GPIO_OUT }, // LED_ACIN

    { &GPCRD0, GPIO_OUT }, // LED_PWR
    { &GPCRD1, GPIO_OUT }, // CCD_EN
    { &GPCRD2, GPIO_ALT }, // BUF_PLT_RST#
    { &GPCRD3, GPIO_IN }, // SCI#
    { &GPCRD4, GPIO_IN }, // SMI#
    { &GPCRD5, GPIO_OUT }, // PWR_BTN#
    { &GPCRD6, GPIO_ALT }, // CPU_FANSEN
    { &GPCRD7, GPIO_IN }, // SUSWARN#

    { &GPCRE0, GPIO_ALT }, // SMC_BAT
    { &GPCRE1, GPIO_OUT }, // AC_PRESENT
    { &GPCRE2, GPIO_IN }, // LEDKB_DET#
    { &GPCRE3, GPIO_OUT }, // USB_PWR_EN#
    { &GPCRE4, GPIO_OUT }, // DD_ON
    { &GPCRE5, GPIO_OUT }, // EC_RSMRST#
    { &GPCRE6, GPIO_IN }, // SB_KBCRST#
    { &GPCRE7, GPIO_ALT }, // SMD_BAT

    { &GPCRF0, GPIO_IN }, // 80CLK
    { &GPCRF1, GPIO_OUT }, // USB_CHARGE_EN
    { &GPCRF2, GPIO_IN | GPIO_UP }, // 3IN1
    { &GPCRF3, GPIO_OUT }, // EC_BT_EN
    { &GPCRF4, GPIO_ALT }, // TP_CLK
    { &GPCRF5, GPIO_ALT }, // TP_DATA
    { &GPCRF6, GPIO_ALT }, // H_PECI
    { &GPCRF7, GPIO_IN }, // CPU_C10_GATE#

    { &GPCRG0, GPIO_OUT }, // NC
    { &GPCRG1, GPIO_OUT }, // WLAN_EN
    { &GPCRG2, GPIO_OUT }, // Pull up to VDD3?
    { &GPCRG3, GPIO_ALT }, // ALSPI_CE#
    { &GPCRG4, GPIO_ALT }, // ALSPI_MSI
    { &GPCRG5, GPIO_ALT }, // ALSPI_MSO
    { &GPCRG6, GPIO_OUT }, // H_PROCHOT_EC
    { &GPCRG7, GPIO_ALT }, // ALSPI_SCLK

    { &GPCRH0, GPIO_ALT }, // EC_CLKRUN#
    { &GPCRH1, GPIO_IN }, // SUSC#_PCH
    { &GPCRH2, GPIO_OUT }, // BKL_EN
    { &GPCRH3, GPIO_OUT }, // EC_GPIO
    { &GPCRH4, GPIO_IN }, // VR_ON
    { &GPCRH5, GPIO_IN }, // SINK_CTRL_EC
    { &GPCRH6, GPIO_IN }, // SUSB#_PCH
    { &GPCRH7, GPIO_IN }, // NC

    { &GPCRI0, GPIO_ALT }, // BAT_DET
    { &GPCRI1, GPIO_ALT }, // BAT_VOLT
    { &GPCRI2, GPIO_OUT }, // ME_WE
    { &GPCRI3, GPIO_ALT }, // THERM_VOLT
    { &GPCRI4, GPIO_ALT }, // TOTAL_CUR
    { &GPCRI5, GPIO_IN }, // NC
    { &GPCRI6, GPIO_IN }, // EC_SMD_EN#
    { &GPCRI7, GPIO_IN }, // MODEL_ID

    { &GPCRJ0, GPIO_IN }, // SLP_S0#
    { &GPCRJ1, GPIO_IN }, // KBC_MUTE#
    { &GPCRJ2, GPIO_ALT }, // KBLIGHT_ADJ
    { &GPCRJ3, GPIO_IN }, // SLP_SUS#
    { &GPCRJ4, GPIO_OUT }, // VA_EC_EN
    { &GPCRJ5, GPIO_IN }, // VBATT_BOOST#
    { &GPCRJ6, GPIO_OUT }, // EC_EN
    { &GPCRJ7, GPIO_IN }, // SUS_PWR_ACK

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
