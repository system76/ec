// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2022 System76, Inc.

#include <board/gpio.h>
#include <common/macro.h>

// uncrustify:off
struct Gpio __code ACIN_N =         GPIO(B, 0);
struct Gpio __code AC_PRESENT =     GPIO(E, 1);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(H, 2);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2); // renamed to EC_ERST#
struct Gpio __code CCD_EN =         GPIO(D, 1);
struct Gpio __code CPU_C10_GATE_N = GPIO(D, 3);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code DGPU_PWR_EN =    GPIO(H, 4);
struct Gpio __code EC_EN =          GPIO(B, 6); // renamed to SUSBC_EN
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code GC6_FB_EN =      GPIO(J, 3);
struct Gpio __code LAN_WAKEUP_N =   GPIO(B, 2);
struct Gpio __code LED_ACIN =       GPIO(C, 7);
struct Gpio __code LED_BAT_CHG =    GPIO(H, 5);
struct Gpio __code LED_BAT_FULL =   GPIO(J, 0);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code ME_WE =          GPIO(A, 0);
struct Gpio __code PCH_DPWROK_EC =  GPIO(C, 5);
struct Gpio __code PCH_PWROK_EC =   GPIO(A, 6);
struct Gpio __code PD_EN =          GPIO(F, 3);
struct Gpio __code PM_PWROK =       GPIO(C, 6);
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code SLP_S0_N =       GPIO(A, 5);
struct Gpio __code SLP_SUS_N =      GPIO(J, 7);
struct Gpio __code SMI_N =          GPIO(D, 4);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code SWI_N =          GPIO(B, 5);
struct Gpio __code VA_EC_EN =       GPIO(J, 4);
struct Gpio __code WLAN_EN =        GPIO(G, 1);
struct Gpio __code WLAN_PWR_EN =    GPIO(A, 3);
struct Gpio __code XLP_OUT =        GPIO(B, 4);
// uncrustify:on

static const struct GpioInit __code gpio_cfg_init[] = {
    // General control
    { &GCR9, BIT(5) }, // PWRSW WDT 2 Enable 2
    { &GCR8, BIT(4) }, // PWRSW WDT 2 Enable 1
    { &GCR, 0b10 << 1 }, // Enable LPC reset on GPD2
    { &GCR6, 0 }, // Disable UARTs
    { &GCR15, BIT(4) }, // Enable SMBus channel 4
    { &GCR19, BIT(0) }, // Set GPD2 to 1.8V
    { &GCR20, 0 }, // Set GPF2 and GPF3 to 3.3V
    //{ &GCR22, BIT(7) },
    //{ &GCR23, BIT(0) }, // Set GPM6 power domain to VCC

    // Port data
    { &GPDRA, 0 },
    { &GPDRB, BIT(4) | BIT(3) }, // XLP_OUT, PWR_SW#
    { &GPDRC, BIT(5) }, // PCH_DPWROK_EC
    { &GPDRD, BIT(5) | BIT(4) }, // PWR_BTN#, SMI#
    { &GPDRE, BIT(3) }, // USB_PWR_EN
    { &GPDRF, BIT(6) }, // H_PECI
    { &GPDRG, BIT(6) }, // H_PROCHOT_EC
    { &GPDRH, 0 },
    { &GPDRI, 0 },
    { &GPDRJ, BIT(1) }, // KBC_MUTE#
    { &GPDRM, 0 },

    // Port control
    { &GPCRA0, GPIO_OUT }, // ME_WE
    { &GPCRA1, GPIO_ALT }, // KBC_KEEP
    { &GPCRA2, GPIO_ALT }, // CPU_FAN
    { &GPCRA3, GPIO_OUT }, // WLAN_PWR_EN
    { &GPCRA4, GPIO_ALT }, // VGA_FAN
    { &GPCRA5, GPIO_IN }, // SLP_S0#
    { &GPCRA6, GPIO_OUT }, // PCH_PWROK_EC
    { &GPCRA7, GPIO_OUT }, // PCH_SLP_WLAN#_R

    { &GPCRB0, GPIO_IN | GPIO_UP }, // AC_IN#
    { &GPCRB1, GPIO_IN | GPIO_UP }, // LID_SW#
    { &GPCRB2, GPIO_IN | GPIO_UP }, // EC_LAN_WAKEUP#
    { &GPCRB3, GPIO_IN }, // PWR_SW#
    { &GPCRB4, GPIO_OUT }, // XLP_OUT
    { &GPCRB5, GPIO_IN }, // SWI#
    { &GPCRB6, GPIO_OUT }, // SUSBC_EC

    { &GPCRC0, GPIO_IN }, // ALL_SYS_PWRGD
    { &GPCRC1, GPIO_ALT | GPIO_UP }, // SMC_VGA_THERM
    { &GPCRC2, GPIO_ALT | GPIO_UP }, // SMD_VGA_THERM
    { &GPCRC3, GPIO_IN }, // SINK_CTRL_EC
    { &GPCRC4, GPIO_IN | GPIO_UP }, // CNVI_DET#
    { &GPCRC5, GPIO_OUT }, // PCH_DPWROK_EC
    { &GPCRC6, GPIO_OUT }, // PM_PWROK
    { &GPCRC7, GPIO_OUT }, // LED_ACIN

    { &GPCRD0, GPIO_OUT }, // LED_PWR
    { &GPCRD1, GPIO_OUT | GPIO_UP }, // CCD_EN
    { &GPCRD2, GPIO_ALT }, // EC_ERST#
    { &GPCRD3, GPIO_IN }, // CPU_C10_GATE#
    { &GPCRD4, GPIO_IN }, // SMI#
    { &GPCRD5, GPIO_OUT }, // PWR_BTN#
    { &GPCRD6, GPIO_ALT }, // CPU_FANSEN
    { &GPCRD7, GPIO_ALT }, // VGA_FANSEN

    { &GPCRE0, GPIO_ALT | GPIO_UP }, // SMC_BAT
    { &GPCRE1, GPIO_OUT }, // AC_PRESENT
    { &GPCRE2, GPIO_IN | GPIO_UP }, // KB-DET
    { &GPCRE3, GPIO_OUT }, // USB_PWR_EN# (Actually active high)
    { &GPCRE4, GPIO_OUT | GPIO_DOWN }, // DD_ON
    { &GPCRE5, GPIO_OUT }, // EC_RSMRST#
    { &GPCRE6, GPIO_IN }, // JACK_IN#_EC
    { &GPCRE7, GPIO_ALT | GPIO_UP }, // SMD_BAT

    { &GPCRF0, GPIO_IN }, // 80CLK
    { &GPCRF1, GPIO_OUT | GPIO_UP }, // USB_CHARGE_EN
    { &GPCRF2, GPIO_IN | GPIO_UP }, // 3IN1
    { &GPCRF3, GPIO_OUT }, // PD_EN
    { &GPCRF4, GPIO_OUT | GPIO_DOWN }, // TP_CLK
    { &GPCRF5, GPIO_OUT | GPIO_DOWN }, // TP_DATA
    { &GPCRF6, GPIO_ALT }, // H_PECI
    { &GPCRF7, GPIO_OUT }, // M2M_SSD_PLN# (Not connected)

    { &GPCRG0, GPIO_IN }, // dGPU_GPIO8_OVERT
    { &GPCRG1, GPIO_OUT }, // WLAN_EN
    { &GPCRG2, GPIO_IN }, // 100k pull-up
    { &GPCRG3, GPIO_ALT }, // ALSPI_CE#
    { &GPCRG4, GPIO_ALT }, // ALSPI_MSI
    { &GPCRG5, GPIO_ALT }, // ALSPI_MSO
    { &GPCRG6, GPIO_OUT }, // H_PROCHOT_EC
    { &GPCRG7, GPIO_ALT }, // ALSPI_SCLK

    { &GPCRH0, GPIO_IN }, // PM_CLKRUN# (Not connected)
    { &GPCRH1, GPIO_IN }, // SUSC#_PCH
    { &GPCRH2, GPIO_OUT }, // BKL_EN
    { &GPCRH3, GPIO_IN }, // PCIE_WAKE#
    { &GPCRH4, GPIO_IN }, // DGPU_PWR_EN
    { &GPCRH5, GPIO_OUT }, // LED_BAT_CHG
    { &GPCRH6, GPIO_IN }, // SUSB#_PCH
    { &GPCRH7, GPIO_IN }, // d_GPIO9_ALERT_FAN

    { &GPCRI0, GPIO_ALT }, // BAT_DET
    { &GPCRI1, GPIO_ALT }, // BAT_VOLT
    { &GPCRI2, GPIO_ALT }, // THERM_VOLT2
    { &GPCRI3, GPIO_ALT }, // THERM_VOLT
    { &GPCRI4, GPIO_ALT }, // TOTAL_CUR
    { &GPCRI5, GPIO_IN }, // Not connected
    { &GPCRI6, GPIO_IN }, // Not connected
    { &GPCRI7, GPIO_IN }, // MODEL_ID

    { &GPCRJ0, GPIO_OUT }, //  LED_BAT_FULL
    { &GPCRJ1, GPIO_OUT }, // KBC_MUTE#
    { &GPCRJ2, GPIO_ALT }, // KBLIGHT_ADJ
    { &GPCRJ3, GPIO_IN }, // GC6_FB_EN_PCH
    { &GPCRJ4, GPIO_OUT }, // VA_EC_EN
    { &GPCRJ5, GPIO_OUT }, // VBATT_BOOST#
    { &GPCRJ6, GPIO_OUT | GPIO_UP }, // EC_GPIO
    { &GPCRJ7, GPIO_IN }, // SLP_SUS#

    { &GPCRM0, GPIO_ALT | GPIO_UP | GPIO_DOWN }, // ESPI_IO0_EC
    { &GPCRM1, GPIO_ALT | GPIO_UP | GPIO_DOWN }, // ESPI_IO1_EC
    { &GPCRM2, GPIO_ALT | GPIO_UP | GPIO_DOWN }, // ESPI_IO2_EC
    { &GPCRM3, GPIO_ALT | GPIO_UP | GPIO_DOWN }, // ESPI_IO3_EC
    { &GPCRM4, GPIO_ALT | GPIO_UP | GPIO_DOWN }, // ESPI_CLK_EC
    { &GPCRM5, GPIO_ALT }, // ESPI_CS_EC#
    { &GPCRM6, GPIO_IN | GPIO_UP | GPIO_DOWN }, // ESPI_ALRT0#
};

void gpio_init(void) {
    for (uint8_t i = 0; i < ARRAY_SIZE(gpio_cfg_init); i++) {
        *gpio_cfg_init[i].reg = gpio_cfg_init[i].data;
    }
}
