// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/macro.h>

// clang-format off
struct Gpio __code ACIN_N =         GPIO(B, 0);
struct Gpio __code AC_PRESENT =     GPIO(F, 7);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(C, 7);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2);
struct Gpio __code CCD_EN =         GPIO(D, 1);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code DGPU_PWR_EN =    GPIO(H, 4);
struct Gpio __code EC_EN =          GPIO(B, 6);
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code GC6_FB_EN =      GPIO(J, 3);
struct Gpio __code H_PROCHOT_EC =   GPIO(G, 6);
struct Gpio __code JACK_IN_N =      GPIO(C, 6);
struct Gpio __code LAN_WAKEUP_N =   GPIO(B, 2);
struct Gpio __code LED_ACIN =       GPIO(H, 2);
struct Gpio __code LED_BAT_CHG =    GPIO(H, 5);
struct Gpio __code LED_BAT_FULL =   GPIO(J, 0);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code PCH_PWROK_EC =   GPIO(F, 3);
struct Gpio __code PD_EN =          GPIO(D, 4);
struct Gpio __code PD_IRQ =         GPIO(E, 2);
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code RGBKB_DET_N =    GPIO(I, 2);
struct Gpio __code SINK_CTRL =      GPIO(H, 7);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 0);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code VA_EC_EN =       GPIO(J, 4);
struct Gpio __code WLAN_PWR_EN =    GPIO(E, 1);
struct Gpio __code XLP_OUT =        GPIO(B, 4);
// clang-format on

void gpio_init(void) {
    // Set global configuration
    GCR = 0x04;
    GCR1 = 0x00;
    GCR2 = 0x00;
    GCR3 = 0x40;
    GCR4 = 0x00;
    GCR5 = 0x00;
    GCR6 = 0x00;
    GCR7 = 0x00;
    GCR8 = 0x10;
    GCR9 = 0x20;
    GCR10 = 0x02;
    GCR11 = 0x00;
    GCR12 = 0x00;
    GCR13 = 0x00;
    GCR14 = 0x00;
    GCR15 = 0x10;
    GCR16 = 0x00;
    GCR17 = 0x00;
    GCR18 = 0x00;
    GCR19 = 0x81;
    GCR20 = 0x80;
    GCR21 = 0x66;
    GCR22 = 0x80;
    GCR23 = 0x01;

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
    // BL_PWM_EN_EC, PLVDD_RST_EC
    GPDRH = BIT(6) | BIT(3);
    GPDRI = 0;
    // KBC_MUTE#
    GPDRJ = BIT(1);
    GPDRM = 0;

    // Set GPIO control

    // EC_PWM_LEDKB_P
    GPCRA0 = GPIO_ALT;
    // KBC_BEEP
    GPCRA1 = GPIO_ALT;
    // CPU_FAN
    GPCRA2 = GPIO_ALT;
    // VGA_FAN
    GPCRA3 = GPIO_ALT;
    // DDS_EC_PWM
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
    GPCRC3 = GPIO_IN;
    // CNVI_DET#
    GPCRC4 = GPIO_IN | GPIO_UP;
    // KB-SO17
    GPCRC5 = GPIO_IN;
    // JACK_IN#_EC
    GPCRC6 = GPIO_IN;
    // BKL_EN
    GPCRC7 = GPIO_OUT | GPIO_UP;

    // LED_PWR
    GPCRD0 = GPIO_OUT;
    // CCD_EN
    GPCRD1 = GPIO_OUT | GPIO_UP;
    // ESPI_RESET_N
    GPCRD2 = GPIO_ALT;
    // SLP_A#
    GPCRD3 = GPIO_IN;
    // PD_POWER_EN
    GPCRD4 = GPIO_OUT;
    // PWR_BTN#
    GPCRD5 = GPIO_OUT;
    // CPU_FANSEN
    GPCRD6 = GPIO_ALT | GPIO_DOWN;
    // VGA_FANSEN
    GPCRD7 = GPIO_ALT | GPIO_DOWN;

    // SMC_BAT
    GPCRE0 = GPIO_ALT | GPIO_UP;
    // WLAN_PWR_EN
    GPCRE1 = GPIO_OUT;
    // TBT_I2C_IRQ2Z
    GPCRE2 = GPIO_IN;
    // USB_PWR_EN
    GPCRE3 = GPIO_OUT | GPIO_UP;
    // DD_ON
    GPCRE4 = GPIO_OUT;
    // EC_RSMRST#
    GPCRE5 = GPIO_OUT;
    // ME_WE
    GPCRE6 = GPIO_OUT;
    // SMD_BAT
    GPCRE7 = GPIO_ALT | GPIO_UP;

    // 80CLK
    GPCRF0 = GPIO_OUT | GPIO_UP;
    // USB_CHARGE_EN
    GPCRF1 = GPIO_OUT;
    // 3IN1
    GPCRF2 = GPIO_OUT | GPIO_UP;
    // PCH_PWROK_EC
    GPCRF3 = GPIO_OUT | GPIO_UP;
    // TP_CLK
    GPCRF4 = GPIO_ALT | GPIO_UP;
    // TP_DATA
    GPCRF5 = GPIO_ALT | GPIO_UP;
    // H_PECI
    GPCRF6 = GPIO_ALT;
    // AC_PRESENT
    GPCRF7 = GPIO_OUT | GPIO_UP;

    // dGPU_GPIO8_OVERT
    GPCRG0 = GPIO_IN | GPIO_UP;
    // MUX_CTRL_BIOS
    GPCRG1 = GPIO_OUT;
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
    GPCRH2 = GPIO_OUT | GPIO_UP;
    // PLVDD_RST_EC
    GPCRH3 = GPIO_OUT;
    // DGPU_PWR_EN
    GPCRH4 = GPIO_IN;
    // LED_BAT_CHG
    GPCRH5 = GPIO_OUT;
    // BL_PWM_EN_EC
    GPCRH6 = GPIO_OUT;
    // SINK_CTRL_EC
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
    // THERM_VOLT3
    GPCRI5 = GPIO_ALT;
    // THERM_VOLT2
    GPCRI6 = GPIO_ALT;
    // MODEL_ID
    GPCRI7 = GPIO_ALT;

    // LED_BAT_FULL
    GPCRJ0 = GPIO_OUT;
    // KBC_MUTE#
    GPCRJ1 = GPIO_OUT;
    // KBLIGHT_ADJ
    GPCRJ2 = GPIO_ALT;
    // GC6_FB_EN_PCH
    GPCRJ3 = GPIO_IN;
    // VA_EC_EN
    GPCRJ4 = GPIO_OUT;
    // VBATT_BOOST#
    GPCRJ5 = GPIO_OUT;
    // EC_GPIO
    GPCRJ6 = GPIO_OUT | GPIO_UP;
    // KB-DET
    GPCRJ7 = GPIO_IN | GPIO_UP;

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
