// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/macro.h>

// uncrustify:off
struct Gpio __code ACIN_N =         GPIO(B, 0);
struct Gpio __code AC_PRESENT =     GPIO(A, 4);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(C, 7);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2); // renamed to ESPI_RESET#
struct Gpio __code CCD_EN =         GPIO(D, 1);
struct Gpio __code CPU_C10_GATE_N = GPIO(F, 7);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code EC_EN =          GPIO(B, 6); // renamed to SUSBC_EC#
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code LED_ACIN =       GPIO(H, 2);
struct Gpio __code LED_PWR =        GPIO(D, 4);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code ME_WE =          GPIO(D, 7);
struct Gpio __code PCH_PWROK_EC =   GPIO(F, 3);
struct Gpio __code PD_EN =          GPIO(D, 0); // renamed to PD_POWER_EN
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code SLP_S0_N =       GPIO(B, 5);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 0);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code VA_EC_EN =       GPIO(J, 4);
struct Gpio __code WLAN_PWR_EN =    GPIO(A, 3);
struct Gpio __code XLP_OUT =        GPIO(B, 4);
// uncrustify:on

static const struct GpioInit __code gpio_cfg_init[] = {
    // General control
    { &GCR, 0b10 << 1 }, // Enable LPC reset on GPD2
    { &GCR2, 0 }, // Disable PECI
    { &GCR6, 0 }, // Disable UARTs
    { &GCR8, BIT(4) }, // PWRSW WDT 2 Enable 1
    { &GCR9, BIT(5) }, // PWRSW WDT 2 Enable 2
    { &GCR10, BIT(1) }, // PWRSW counter 12 seconds
    { &GCR15, BIT(4) }, // Enable SMBus channel 4
    { &GCR19, BIT(7) | BIT(0) }, // Set GPB5 and GPD2 to 1.8V
    { &GCR20, BIT(7) }, // Set GPD3 to 1.8V, GPF2 and GPF3 to 3.3V
    { &GCR21, BIT(5) | BIT(2) | BIT(1) }, // Set GPF7, GPH0, and GPH1 to 1.8V
    { &GCR22, BIT(7) },
    { &GCR23, BIT(0) }, // Set GPM6 power domain to VCC

    // Port data
    { &GPDRA, 0 },
    { &GPDRB, BIT(4) }, // XLP_OUT
    { &GPDRC, 0 },
    { &GPDRD, BIT(5) }, // PWR_BTN#
    { &GPDRE, BIT(3) }, // USB_PWR_EN
    { &GPDRF, BIT(6) }, // H_PECI
    { &GPDRG, BIT(6) }, // H_PROCHOT_EC
    { &GPDRH, 0 },
    { &GPDRI, 0 },
    { &GPDRJ, BIT(1) }, // KBC_MUTE#
    { &GPDRM, 0 },

    // Port control
    { &GPCRA0, GPIO_OUT }, // EC_PWM_LEDKB_P
    { &GPCRA1, GPIO_IN }, // KBC_BEEP
    { &GPCRA2, GPIO_ALT }, // CPU_FAN
    { &GPCRA3, GPIO_OUT }, // WLAN_PWR_EN
    { &GPCRA4, GPIO_OUT }, // AC_PRESENT_EC
    { &GPCRA5, GPIO_ALT }, // EC_PWM_LEDKB_R
    { &GPCRA6, GPIO_ALT }, // EC_PWM_LEDKB_G
    { &GPCRA7, GPIO_IN }, // TBTA_VBUS_2_EN#

    { &GPCRB0, GPIO_IN | GPIO_UP }, // AC_IN#
    { &GPCRB1, GPIO_IN | GPIO_UP }, // LID_SW#
    { &GPCRB2, GPIO_IN }, // PCIE_WAKE#
    { &GPCRB3, GPIO_IN }, // PWR_SW#
    { &GPCRB4, GPIO_OUT }, // XLP_OUT
    { &GPCRB5, GPIO_IN }, // SLP_S0#
    { &GPCRB6, GPIO_OUT }, // SUSBC_EC#

    { &GPCRC0, GPIO_IN }, // ALL_SYS_PWRGD
    { &GPCRC1, GPIO_ALT | GPIO_UP }, // SMB_CLK_EC
    { &GPCRC2, GPIO_ALT | GPIO_UP }, // SMB_DATA_EC
    { &GPCRC3, GPIO_ALT | GPIO_UP }, // KB-SO16
    { &GPCRC4, GPIO_IN | GPIO_UP }, // CNVI_DET#
    { &GPCRC5, GPIO_ALT | GPIO_UP }, // KB-SO17
    { &GPCRC6, GPIO_IN }, // TMRI1-TEST
    { &GPCRC7, GPIO_OUT }, // BKL_EN

    { &GPCRD0, GPIO_OUT }, // PD_POWER_EN
    { &GPCRD1, GPIO_OUT }, // CCD_EN
    { &GPCRD2, GPIO_ALT }, // ESPI_RESET#
    { &GPCRD3, GPIO_IN }, // SLP_A#
    { &GPCRD4, GPIO_OUT }, // LED_PWR
    { &GPCRD5, GPIO_OUT }, // EC_PWR_BTN#
    { &GPCRD6, GPIO_ALT | GPIO_DOWN }, // CPU_FANSEN
    { &GPCRD7, GPIO_OUT }, // EC_ME_WE

    { &GPCRE0, GPIO_ALT | GPIO_UP }, // SMC_BAT
    { &GPCRE1, GPIO_IN }, // GPE1_TEST
    { &GPCRE2, GPIO_IN }, // ACE_I2C_IRQ2Z
    { &GPCRE3, GPIO_OUT }, // USB_PWR_EN
    { &GPCRE4, GPIO_OUT }, // DD_ON
    { &GPCRE5, GPIO_OUT }, // EC_RSMRST#
    { &GPCRE6, GPIO_IN }, // JACK_IN#_EC
    { &GPCRE7, GPIO_ALT | GPIO_UP }, // SMD_BAT

    { &GPCRF0, GPIO_OUT }, // 80CLK
    { &GPCRF1, GPIO_OUT }, // USB_CHARGE_EN
    { &GPCRF2, GPIO_OUT }, // 3IN1
    { &GPCRF3, GPIO_OUT }, // PCH_PWROK_EC
    { &GPCRF4, GPIO_ALT | GPIO_UP }, // TP_CLK
    { &GPCRF5, GPIO_ALT | GPIO_UP }, // TP_DATA
    { &GPCRF6, GPIO_ALT }, // H_PECI
    { &GPCRF7, GPIO_IN }, // CPU_C10_GATE#

    { &GPCRG0, GPIO_IN }, // 10k pull-down
    { &GPCRG1, GPIO_IN }, // WLAN_EN
    { &GPCRG2, GPIO_IN }, // Pull up to VDD3
    { &GPCRG3, GPIO_ALT }, // ALSPI_CE#
    { &GPCRG4, GPIO_ALT }, // ALSPI_MSI
    { &GPCRG5, GPIO_ALT }, // ALSPI_MSO
    { &GPCRG6, GPIO_OUT }, // H_PROCHOT_EC
    { &GPCRG7, GPIO_ALT }, // ALSPI_SCLK

    { &GPCRH0, GPIO_IN }, // SUSB#_PCH
    { &GPCRH1, GPIO_IN }, // SUSC#_PCH
    { &GPCRH2, GPIO_OUT }, // LED_ACIN
    { &GPCRH3, GPIO_OUT }, // 3G_EN
    { &GPCRH4, GPIO_OUT }, // 3G_PWR_EN
    { &GPCRH5, GPIO_IN }, // TBTA_VBUS_1_EN#
    { &GPCRH6, GPIO_IN }, // Not connected
    { &GPCRH7, GPIO_IN }, // Not connected

    { &GPCRI0, GPIO_ALT }, // BAT_DET
    { &GPCRI1, GPIO_ALT }, // BAT_VOLT
    { &GPCRI2, GPIO_IN | GPIO_UP }, // RGBKB-DET#
    { &GPCRI3, GPIO_ALT }, // THERM_VOLT_CPU_1
    { &GPCRI4, GPIO_ALT }, // TOTAL_CUR
    { &GPCRI5, GPIO_IN }, // Not connected
    { &GPCRI6, GPIO_ALT }, // THERM_VOLT_CPU_2
    { &GPCRI7, GPIO_IN }, // MODEL_ID

    { &GPCRJ0, GPIO_IN }, // SINK_CTRL_EC_1
    { &GPCRJ1, GPIO_OUT }, // KBC_MUTE#
    { &GPCRJ2, GPIO_ALT }, // KBLIGHT_ADJ
    { &GPCRJ3, GPIO_IN }, // SINK_CTRL_EC_2
    { &GPCRJ4, GPIO_OUT }, // VA_EC_EN
    { &GPCRJ5, GPIO_IN }, // VBATT_BOOST#
    { &GPCRJ6, GPIO_OUT }, // EC_GPIO
    { &GPCRJ7, GPIO_IN | GPIO_UP }, // LEDKB_DET#

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
