// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/macro.h>

// clang-format off
struct Gpio __code ACIN_N =         GPIO(B, 0);
struct Gpio __code AC_PRESENT =     GPIO(F, 7);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(C, 7);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2); // renamed to ESPI_RESET#
struct Gpio __code CCD_EN =         GPIO(D, 1);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code EC_EN =          GPIO(B, 6); // renamed to SUSBC_EC
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code LAN_WAKEUP_N =   GPIO(B, 2);
struct Gpio __code LED_ACIN =       GPIO(H, 2);
struct Gpio __code LED_BAT_CHG =    GPIO(H, 5);
struct Gpio __code LED_BAT_FULL =   GPIO(J, 0);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code ME_WE =          GPIO(E, 6);
struct Gpio __code PCH_PWROK_EC =   GPIO(F, 3);
struct Gpio __code PD_EN =          GPIO(D, 4); // renamed to PD_POWER_EN
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code RGBKB_DET_N =    GPIO(I, 2);
struct Gpio __code SLP_S0_N =       GPIO(B, 5);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 0);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code VA_EC_EN =       GPIO(J, 4);
struct Gpio __code WLAN_PWR_EN =    GPIO(E, 1);
struct Gpio __code XLP_OUT =        GPIO(B, 4);
// clang-format on

void gpio_init(void) {
    // PWRSW WDT 2 Enable 2
    GCR9 = BIT(5);
    // PWRSW WDT 2 Enable 1
    GCR8 = BIT(4);

    // Enable LPC reset on GPD2
    GCR = 0b10 << 1;
    // Disable UARTs
    GCR6 = 0;
    // Enable SMBus channel 4
    GCR15 = BIT(4);
    // Set GPB5 and GPD2 to 1.8V
    GCR19 = BIT(7) | BIT(0);
    // Set GPD3 to 1.8V, GPF2 and GPF3 to 3.3V
    GCR20 = BIT(7);
    // Set GPF7, GPH0, and GPH1 to 1.8V
    GCR21 = BIT(5) | BIT(2) | BIT(1);
    // Not documented
    GCR22 = BIT(7);
    // Set GPM6 power domain to VCC
    GCR23 = BIT(0);

    // Set GPIO data
    GPDRA = 0;
    // XLP_OUT, PWR_SW#
    GPDRB = BIT(4) | BIT(3);
    GPDRC = 0;
    // PWR_BTN#
    GPDRD = BIT(5);
    // USB_PWR_EN
    GPDRE = BIT(3);
    // H_PECI
    GPDRF = BIT(6);
    // H_PROCHOT_EC
    GPDRG = BIT(6);
    GPDRH = 0;
    GPDRI = 0;
    // KBC_MUTE#
    GPDRJ = BIT(1);
    GPDRM = 0;

    // Set GPIO control

    // EC_PWM_LEDKB_P
    GPCRA0 = GPIO_ALT;
    // KBC_BEEP (NC)
    GPCRA1 = GPIO_IN;
    // CPU_FAN1
    GPCRA2 = GPIO_ALT;
    // CPU_FAN2
    GPCRA3 = GPIO_ALT;
    // NC
    GPCRA4 = GPIO_IN;
    // EC_PWM_LEDKB_R
    GPCRA5 = GPIO_ALT;
    // EC_PWM_LEDKB_G
    GPCRA6 = GPIO_ALT;
    // EC_PWM_LEDKB_B
    GPCRA7 = GPIO_ALT;

    // AC_IN#
    GPCRB0 = GPIO_IN | GPIO_UP;
    // LID_SW#
    GPCRB1 = GPIO_IN | GPIO_UP;
    // EC_LAN_WAKEUP#
    GPCRB2 = GPIO_IN | GPIO_UP;
    // PWR_SW#
    GPCRB3 = GPIO_IN;
    // XLP_OUT
    GPCRB4 = GPIO_OUT;
    // SLP_S0#
    GPCRB5 = GPIO_IN;
    // SUSBC_EC
    GPCRB6 = GPIO_OUT;

    // ALL_SYS_PWRGD
    GPCRC0 = GPIO_IN;
    // SMC_VGA_THERM
    GPCRC1 = GPIO_ALT | GPIO_UP;
    // SMD_VGA_THERM
    GPCRC2 = GPIO_ALT | GPIO_UP;
    // KB-SO16
    GPCRC3 = GPIO_ALT | GPIO_UP;
    // CNVI_DET#
    GPCRC4 = GPIO_IN | GPIO_UP;
    // KB-SO17
    GPCRC5 = GPIO_ALT | GPIO_UP;
    // JACK_IN#_EC
    GPCRC6 = GPIO_IN;
    // BKL_EN
    GPCRC7 = GPIO_OUT;

    // LED_PWR
    GPCRD0 = GPIO_OUT;
    // CCD_EN
    GPCRD1 = GPIO_OUT;
    // ESPI_RESET_N
    GPCRD2 = GPIO_ALT;
    // SLP_A#
    GPCRD3 = GPIO_IN;
    // PD_POWER_EN
    GPCRD4 = GPIO_OUT;
    // PWR_BTN#
    GPCRD5 = GPIO_OUT;
    // CPU_FANSEN1
    GPCRD6 = GPIO_ALT;
    // CPU_FANSEN2
    GPCRD7 = GPIO_ALT;

    // SMC_BAT
    GPCRE0 = GPIO_ALT | GPIO_UP;
    // WLAN_PWR_EN
    GPCRE1 = GPIO_OUT;
    // TBT_I2C_IRQ2Z
    GPCRE2 = GPIO_IN;
    // USB_PWR_EN
    GPCRE3 = GPIO_OUT;
    // DD_ON
    GPCRE4 = GPIO_OUT;
    // EC_RSMRST#
    GPCRE5 = GPIO_OUT;
    // ME_WE
    GPCRE6 = GPIO_OUT;
    // SMD_BAT
    GPCRE7 = GPIO_ALT | GPIO_UP;

    // 80CLK
    GPCRF0 = GPIO_IN;
    // USB_CHARGE_EN
    GPCRF1 = GPIO_OUT;
    // 3IN1
    GPCRF2 = GPIO_IN;
    // PCH_PWROK_EC
    GPCRF3 = GPIO_OUT;
    // TP_CLK
    GPCRF4 = GPIO_ALT | GPIO_UP;
    // TP_DATA
    GPCRF5 = GPIO_ALT | GPIO_UP;
    // H_PECI
    GPCRF6 = GPIO_ALT;
    // AC_PRESENT
    GPCRF7 = GPIO_OUT;

    // NC
    GPCRG0 = GPIO_IN;
    // NC
    GPCRG1 = GPIO_IN;
    // 100k pull-up to VDD3
    GPCRG2 = GPIO_IN;
    // HSPI_CE#
    GPCRG3 = GPIO_ALT;
    // HSPI_MSI
    GPCRG4 = GPIO_ALT;
    // HSPI_MSO
    GPCRG5 = GPIO_ALT;
    // H_PROCHOT_EC
    GPCRG6 = GPIO_OUT;
    // HSPI_SCLK
    GPCRG7 = GPIO_ALT;

    // SUSB#_PCH
    GPCRH0 = GPIO_IN;
    // SUSC#_PCH
    GPCRH1 = GPIO_IN;
    // LED_ACIN
    GPCRH2 = GPIO_OUT;
    // NC
    GPCRH3 = GPIO_IN;
    // NC
    GPCRH4 = GPIO_IN;
    // LED_BAT_CHG
    GPCRH5 = GPIO_OUT;
    // NC
    GPCRH6 = GPIO_IN;
    // NC
    GPCRH7 = GPIO_IN;

    // BAT_DET
    GPCRI0 = GPIO_ALT;
    // BAT_VOLT
    GPCRI1 = GPIO_ALT;
    // RGBKB_DET#
    GPCRI2 = GPIO_IN | GPIO_UP;
    // THERM_VOLT
    GPCRI3 = GPIO_ALT;
    // TOTAL_CUR
    GPCRI4 = GPIO_ALT;
    // EC_CCD_WP#
    GPCRI5 = GPIO_OUT;
    // THERM_VOLT2
    GPCRI6 = GPIO_ALT;
    // MODEL_ID
    GPCRI7 = GPIO_IN;

    // LED_BAT_FULL
    GPCRJ0 = GPIO_OUT;
    // KBC_MUTE#
    GPCRJ1 = GPIO_OUT;
    // KBLIGHT_ADJ
    GPCRJ2 = GPIO_ALT;
    // SINK_CTRL_EC_1
    GPCRJ3 = GPIO_IN;
    // VA_EC_EN
    GPCRJ4 = GPIO_OUT;
    // SINK_CTRL_EC_2
    GPCRJ5 = GPIO_IN;
    // EC_GPIO
    GPCRJ6 = GPIO_OUT;
    // KB-DET
    GPCRJ7 = GPIO_IN;

    // ESPI_IO0_EC
    GPCRM0 = GPIO_ALT | GPIO_UP | GPIO_DOWN;
    // ESPI_IO1_EC
    GPCRM1 = GPIO_ALT | GPIO_UP | GPIO_DOWN;
    // ESPI_IO2_EC
    GPCRM2 = GPIO_ALT | GPIO_UP | GPIO_DOWN;
    // ESPI_IO3_EC
    GPCRM3 = GPIO_ALT | GPIO_UP | GPIO_DOWN;
    // ESPI_CLK_EC
    GPCRM4 = GPIO_ALT | GPIO_UP | GPIO_DOWN;
    // ESPI_CS_EC#
    GPCRM5 = GPIO_ALT;
    // ESPI_ALRT0#
    GPCRM6 = GPIO_IN | GPIO_UP | GPIO_DOWN;
}
