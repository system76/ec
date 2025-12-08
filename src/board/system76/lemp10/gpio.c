// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

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
struct Gpio __code CPU_C10_GATE_N = GPIO(F, 7);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code EC_EN =          GPIO(J, 6);
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
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
struct Gpio __code SLP_S0_N =       GPIO(J, 0);
struct Gpio __code SLP_SUS_N =      GPIO(J, 3);
struct Gpio __code SMI_N =          GPIO(D, 4);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
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
    { &GCR9, BIT(5) }, // PWRSW WDT 2 Enable 2
    { &GCR8, BIT(4) }, // PWRSW WDT 2 Enable 1
    { &GCR, 0x04 }, // Enable LPC reset on GPD2
    { &GCR15, BIT(4) }, // Enable SMBus channel 4
    { &GCR19, BIT(0) }, // Set GPD2 to 1.8V
    { &GCR20, 0 }, // Set GPF2 and GPF3 to 3.3V
    { &GCR21, BIT(2) }, // Set GPH0 to 1.8V

    // Port data
    { &GPDRA, 0 },
    { &GPDRB, BIT(4) | BIT(3) }, // XLP_OUT, PWR_SW#
    { &GPDRC, 0 },
    { &GPDRD, BIT(5) | BIT(4) }, // PWR_BTN#, SMI#
    { &GPDRE, BIT(3) }, // USB_PWR_EN#
    { &GPDRF, BIT(6) }, // H_PECI
    { &GPDRG, 0 },
    { &GPDRH, 0 },
    { &GPDRI, 0 },
    { &GPDRJ, BIT(1) }, // KBC_MUTE#
    { &GPDRM, 0 },

    // Port control
    { &GPCRA0, GPIO_IN }, // AC/BATL#
    { &GPCRA1, GPIO_ALT }, // KBC_BEEP
    { &GPCRA2, GPIO_ALT }, // CPU_FAN
    { &GPCRA3, GPIO_OUT | GPIO_UP }, // WLAN_PWR_EN
    { &GPCRA4, GPIO_IN }, // NC
    { &GPCRA5, GPIO_OUT | GPIO_UP }, // NC
    { &GPCRA6, GPIO_OUT | GPIO_UP }, // PCH_PWROK_EC
    { &GPCRA7, GPIO_OUT | GPIO_UP }, // PCH_DPWROK_EC

    { &GPCRB0, GPIO_IN | GPIO_UP }, // AC_IN#
    { &GPCRB1, GPIO_IN | GPIO_UP }, // LID_SW#
    { &GPCRB2, GPIO_IN | GPIO_UP }, // PCH_SLP_WLAN#_R
    { &GPCRB3, GPIO_IN }, // PWR_SW#
    { &GPCRB4, GPIO_OUT }, // XLP_OUT
    { &GPCRB5, GPIO_OUT | GPIO_UP }, // SWI#
    { &GPCRB6, GPIO_IN }, // EC_FORCE_POWER

    { &GPCRC0, GPIO_IN }, // ALL_SYS_PWRGD
    { &GPCRC1, GPIO_ALT | GPIO_UP }, // SMB_CLK_EC
    { &GPCRC2, GPIO_ALT | GPIO_UP }, // SMB_DATA_EC
    { &GPCRC3, GPIO_IN }, // PCIE_WAKE#
    { &GPCRC4, GPIO_IN | GPIO_UP }, // CNVI_DET#
    { &GPCRC5, GPIO_OUT | GPIO_UP }, // 3G_PWR_EN
    { &GPCRC6, GPIO_OUT }, // PM_PWROK
    { &GPCRC7, GPIO_OUT | GPIO_UP }, // LED_ACIN

    { &GPCRD0, GPIO_OUT | GPIO_UP }, // LED_PWR
    { &GPCRD1, GPIO_OUT | GPIO_UP }, // CCD_EN
    { &GPCRD2, GPIO_ALT }, // BUF_PLT_RST#
    { &GPCRD3, GPIO_IN }, // SCI#
    { &GPCRD4, GPIO_IN | GPIO_UP }, // SMI#
    { &GPCRD5, GPIO_OUT | GPIO_UP }, // PWR_BTN#
    { &GPCRD6, GPIO_ALT }, // CPU_FANSEN
    { &GPCRD7, GPIO_IN }, // NC

    { &GPCRE0, GPIO_ALT }, // SMC_BAT
    { &GPCRE1, GPIO_OUT | GPIO_UP }, // AC_PRESENT
    { &GPCRE2, GPIO_IN }, // LEDKB_DET#
    { &GPCRE3, GPIO_OUT | GPIO_UP }, // USB_PWR_EN#
    { &GPCRE4, GPIO_OUT | GPIO_DOWN }, // DD_ON
    { &GPCRE5, GPIO_OUT }, // EC_RSMRST#
    { &GPCRE6, GPIO_IN }, // SB_KBCRST#
    { &GPCRE7, GPIO_ALT }, // SMD_BAT

    { &GPCRF0, GPIO_IN | GPIO_UP }, // 80CLK
    { &GPCRF1, GPIO_OUT | GPIO_UP }, // USB_CHARGE_EN
    { &GPCRF2, GPIO_IN | GPIO_UP }, // 3IN1
    { &GPCRF3, GPIO_OUT | GPIO_UP }, // EC_BT_EN
    { &GPCRF4, GPIO_ALT | GPIO_UP }, // TP_CLK
    { &GPCRF5, GPIO_ALT | GPIO_UP }, // TP_DATA
    { &GPCRF6, GPIO_ALT }, // H_PECI
    { &GPCRF7, GPIO_IN | GPIO_DOWN }, // CPU_C10_GATE#

    { &GPCRG0, GPIO_IN }, // VCCIN_AUX_PG
    { &GPCRG1, GPIO_OUT | GPIO_UP }, // WLAN_EN
    { &GPCRG2, GPIO_OUT }, // Pull up to VDD3?
    { &GPCRG3, GPIO_ALT }, // ALSPI_CE#
    { &GPCRG4, GPIO_ALT }, // ALSPI_MSI
    { &GPCRG5, GPIO_ALT }, // ALSPI_MSO
    { &GPCRG6, GPIO_OUT | GPIO_UP }, // H_PROCHOT_EC
    { &GPCRG7, GPIO_ALT }, // ALSPI_SCLK

    { &GPCRH0, GPIO_IN }, // EC_CLKRUN#
    { &GPCRH1, GPIO_IN }, // SUSC#_PCH
    { &GPCRH2, GPIO_OUT | GPIO_UP }, // BKL_EN
    { &GPCRH3, GPIO_OUT | GPIO_UP }, // 3G_EN
    { &GPCRH4, GPIO_IN }, // VR_ON
    { &GPCRH5, GPIO_IN }, // SINK_CTRL_EC
    { &GPCRH6, GPIO_IN }, // SUSB#_PCH
    { &GPCRH7, GPIO_IN }, // ACE_I2C_IRQ2Z

    { &GPCRI0, GPIO_ALT }, // BAT_DET
    { &GPCRI1, GPIO_ALT }, // BAT_VOLT
    { &GPCRI2, GPIO_OUT }, // ME_WE
    { &GPCRI3, GPIO_ALT }, // THERM_VOLT
    { &GPCRI4, GPIO_ALT }, // TOTAL_CUR
    { &GPCRI5, GPIO_IN }, // NC
    { &GPCRI6, GPIO_IN }, // NC
    { &GPCRI7, GPIO_IN }, // MODEL_ID

    { &GPCRJ0, GPIO_IN | GPIO_DOWN }, // SLP_S0#
    { &GPCRJ1, GPIO_OUT }, // KBC_MUTE#
    { &GPCRJ2, GPIO_ALT }, // KBLIGHT_ADJ
    { &GPCRJ3, GPIO_IN }, // SLP_SUS#
    { &GPCRJ4, GPIO_OUT }, // VA_EC_EN
    { &GPCRJ5, GPIO_IN }, // VBATT_BOOST#
    { &GPCRJ6, GPIO_OUT | GPIO_UP }, // EC_EN
    { &GPCRJ7, GPIO_IN }, // NC

    { &GPCRM0, GPIO_ALT }, // ESPI_IO_0
    { &GPCRM1, GPIO_ALT }, // ESPI_IO_1
    { &GPCRM2, GPIO_ALT }, // ESPI_IO_2
    { &GPCRM3, GPIO_ALT }, // ESPI_IO_3
    { &GPCRM4, GPIO_ALT | GPIO_UP | GPIO_DOWN }, // ESPI_CLK
    { &GPCRM5, GPIO_ALT }, // ESPI_CS_N
    { &GPCRM6, GPIO_IN | GPIO_UP | GPIO_DOWN }, // SERIRQ
};

void gpio_init(void) {
    for (uint8_t i = 0; i < ARRAY_SIZE(gpio_cfg_init); i++) {
        *gpio_cfg_init[i].reg = gpio_cfg_init[i].data;
    }
}
