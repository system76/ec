// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2022 System76, Inc.

#include <board/gpio.h>
#include <common/macro.h>

// uncrustify:off
struct Gpio __code ACIN_N =         GPIO(B, 0);
struct Gpio __code AC_PRESENT =     GPIO(E, 1);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(C, 7);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2); // renamed to ESPI_RESET_N
struct Gpio __code CCD_EN =         GPIO(D, 1);
struct Gpio __code CPU_C10_GATE_N = GPIO(C, 6);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code DGPU_PWR_EN =    GPIO(J, 2);
struct Gpio __code EC_EN =          GPIO(B, 6); // renamed to SUSBC_EN#
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code GC6_FB_EN =      GPIO(J, 3);
struct Gpio __code LAN_WAKEUP_N =   GPIO(B, 2);
struct Gpio __code LED_ACIN =       GPIO(H, 2);
struct Gpio __code LED_BAT_CHG =    GPIO(H, 5);
struct Gpio __code LED_BAT_FULL =   GPIO(J, 0);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code ME_WE =          GPIO(E, 6);
struct Gpio __code PCH_DPWROK_EC =  GPIO(H, 4);
struct Gpio __code PCH_PWROK_EC =   GPIO(F, 3);
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code RGBKB_DET_N =    GPIO(E, 2);
struct Gpio __code SLP_S0_N =       GPIO(C, 6); // XXX: Really CPU_C10_GATE#
struct Gpio __code SLP_SUS_N =      GPIO(J, 4);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code VA_EC_EN =       GPIO(H, 7);
struct Gpio __code WLAN_EN =        GPIO(G, 1);
struct Gpio __code WLAN_PWR_EN =    GPIO(A, 3);
struct Gpio __code XLP_OUT =        GPIO(B, 4);
// uncrustify:on

static const struct GpioInit __code gpio_cfg_init[] = {
    // General control
    { &GCR9, BIT(5) }, // PWRSW WDT 2 Enable 2
    { &GCR8, BIT(4) }, // PWRSW WDT 2 Enable 1
    { &GCR, 0x04 }, // Enable LPC reset on GPD2
    { &GCR6, 0 }, // Disable UARTs
    { &GCR15, BIT(4) }, // Enable SMBus channel 4
    { &GCR19, BIT(0) }, // Set GPD2 to 1.8V
    { &GCR20, 0 }, // Set GPF2 and GPF3 to 3.3V

    // Port data
    { &GPDRA, 0 },
    { &GPDRB, BIT(4) }, // XLP_OUT
    { &GPDRC, 0 },
    { &GPDRD, 0 },
    { &GPDRE, BIT(3) }, // USB_PWR_EN
    { &GPDRF, BIT(7) }, // CC_EN
    { &GPDRG, BIT(6) }, // H_PROCHOT#_EC
    { &GPDRH, 0 },
    { &GPDRI, 0 },
    { &GPDRJ, BIT(1) }, // KBC_MUTE#

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
    { &GPCRB5, GPIO_OUT }, // SWI#
    { &GPCRB6, GPIO_OUT | GPIO_UP }, // SUSBC_EC#

    { &GPCRC0, GPIO_IN }, // ALL_SYS_PWRGD
    { &GPCRC1, GPIO_ALT | GPIO_UP }, // SMC_VGA_THERM
    { &GPCRC2, GPIO_ALT | GPIO_UP }, // SMD_VGA_THERM
    { &GPCRC3, GPIO_ALT | GPIO_UP }, // KB_SO16
    { &GPCRC4, GPIO_IN | GPIO_UP }, // CNVI_DET#
    { &GPCRC5, GPIO_ALT | GPIO_UP }, // KB_SO17
    { &GPCRC6, GPIO_IN }, // CPU_C10_GATE#
    { &GPCRC7, GPIO_OUT | GPIO_UP }, // BKL_EN

    { &GPCRD0, GPIO_OUT | GPIO_UP }, // LED_PWR
    { &GPCRD1, GPIO_OUT }, // CCD_EN
    { &GPCRD2, GPIO_ALT }, // ESPI_RESET_N
    { &GPCRD3, GPIO_IN }, // SCI#
    { &GPCRD4, GPIO_IN }, // SMI#
    { &GPCRD5, GPIO_OUT | GPIO_UP }, // PWR_BTN#
    { &GPCRD6, GPIO_ALT }, // CPU_FANSEN
    { &GPCRD7, GPIO_ALT }, // VGA_FANSEN

    { &GPCRE0, GPIO_ALT | GPIO_UP }, // SMC_BAT
    { &GPCRE1, GPIO_OUT | GPIO_UP }, // AC_PRESENT
    { &GPCRE2, GPIO_IN | GPIO_UP }, // RGBKB-DET#
    { &GPCRE3, GPIO_OUT }, // USB_PWR_EN
    { &GPCRE4, GPIO_OUT | GPIO_DOWN }, // DD_ON
    { &GPCRE5, GPIO_OUT }, // EC_RSMRST#
    { &GPCRE6, GPIO_OUT }, // ME_WE
    { &GPCRE7, GPIO_ALT | GPIO_UP }, // SMD_BAT

    { &GPCRF0, GPIO_IN }, // 80CLK
    { &GPCRF1, GPIO_OUT | GPIO_UP }, // USB_CHARGE_EN
    { &GPCRF2, GPIO_IN | GPIO_UP }, // 3IN1
    { &GPCRF3, GPIO_OUT }, // PCH_PWROK_EC
    { &GPCRF4, GPIO_ALT | GPIO_DOWN }, // TP_CLK
    { &GPCRF5, GPIO_ALT | GPIO_DOWN }, // TP_DATA
    { &GPCRF6, GPIO_ALT }, // H_PECI
    { &GPCRF7, GPIO_OUT | GPIO_UP }, // CC_EN

    { &GPCRG0, GPIO_IN }, // dGPU_GPIO8_OVERT
    { &GPCRG1, GPIO_OUT | GPIO_UP }, // WLAN_EN
    { &GPCRG2, GPIO_IN }, // AUTO_LOAD_PWR
    { &GPCRG3, GPIO_ALT }, // ALSPI_CE#
    { &GPCRG4, GPIO_ALT }, // ALSPI_MSI
    { &GPCRG5, GPIO_ALT }, // ALSPI_MSO
    { &GPCRG6, GPIO_OUT | GPIO_UP }, // H_PROCHOT#_EC
    { &GPCRG7, GPIO_ALT }, // ALSPI_SCLK

    { &GPCRH0, GPIO_IN }, // PM_CLKRUN#
    { &GPCRH1, GPIO_IN }, // SUSC#_PCH
    { &GPCRH2, GPIO_OUT | GPIO_UP }, // LED_ACIN
    { &GPCRH3, GPIO_IN | GPIO_UP }, // LIGHT_KB_DET#
    { &GPCRH4, GPIO_OUT }, // PCH_DPWROK_EC
    { &GPCRH5, GPIO_OUT | GPIO_UP }, // LED_BAT_CHG
    { &GPCRH6, GPIO_IN }, // SUSB#_PCH
    { &GPCRH7, GPIO_OUT | GPIO_UP }, // VA_EC_EN

    { &GPCRI0, GPIO_ALT }, // BAT_DET
    { &GPCRI1, GPIO_ALT }, // BAT_VOLT
    { &GPCRI2, GPIO_ALT }, // THERM_VOLT2
    { &GPCRI3, GPIO_ALT }, // THERM_VOLT
    { &GPCRI4, GPIO_ALT }, // TOTAL_CUR
    { &GPCRI5, GPIO_ALT }, // CC1_DET
    { &GPCRI6, GPIO_ALT }, // CC2_DET
    { &GPCRI7, GPIO_IN }, // MODEL_ID

    { &GPCRJ0, GPIO_OUT | GPIO_UP }, // LED_BAT_FULL
    { &GPCRJ1, GPIO_IN }, // KBC_MUTE#
    { &GPCRJ2, GPIO_IN }, // DGPU_PWR_EN
    { &GPCRJ3, GPIO_IN }, // GC6_FB_EN_PCH
    { &GPCRJ4, GPIO_IN }, // SLP_SUS#
    { &GPCRJ5, GPIO_OUT }, // VBATT_BOOST#
    { &GPCRJ6, GPIO_IN }, // EC_GPIO
    { &GPCRJ7, GPIO_IN }, // SLP_S0# (not connected)

    { &GPCRM0, GPIO_ALT }, // ESPI_IO0_EC
    { &GPCRM1, GPIO_ALT }, // ESPI_IO1_EC
    { &GPCRM2, GPIO_ALT }, // ESPI_IO2_EC
    { &GPCRM3, GPIO_ALT }, // ESPI_IO3_EC
    { &GPCRM4, GPIO_ALT }, // ESPI_CLK_EC
    { &GPCRM5, GPIO_ALT }, // ESPI_CS_EC#
    { &GPCRM6, GPIO_IN | GPIO_UP }, // ESPI_ALRT0#
};

void gpio_init(void) {
    for (uint8_t i = 0; i < ARRAY_SIZE(gpio_cfg_init); i++) {
        *gpio_cfg_init[i].reg = gpio_cfg_init[i].data;
    }
}
