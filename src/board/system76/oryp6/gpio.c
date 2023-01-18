// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/macro.h>

// clang-format off
struct Gpio __code ACIN_N =         GPIO(B, 0);
struct Gpio __code AC_PRESENT =     GPIO(E, 1);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(H, 2);
struct Gpio __code BT_EN =          GPIO(F, 3);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2);
struct Gpio __code CCD_EN =         GPIO(D, 1);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code DGPU_PWR_EN =    GPIO(F, 7);
struct Gpio __code EC_EN =          GPIO(B, 6); // renamed to SUSBC_EN
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code GC6_FB_EN =      GPIO(J, 7);
struct Gpio __code LAN_WAKEUP_N =   GPIO(B, 2);
struct Gpio __code LED_ACIN =       GPIO(C, 7);
struct Gpio __code LED_BAT_CHG =    GPIO(H, 5);
struct Gpio __code LED_BAT_FULL =   GPIO(J, 0);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code PM_PWROK =       GPIO(C, 6);
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code SB_KBCRST_N =    GPIO(E, 6);
struct Gpio __code SCI_N =          GPIO(D, 3);
struct Gpio __code SMI_N =          GPIO(D, 4);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code SWI_N =          GPIO(B, 5);
struct Gpio __code USB_PWR_EN_N =   GPIO(E, 3);
struct Gpio __code VA_EC_EN =       GPIO(J, 4); // renamed to EC_SLP_SUS#
struct Gpio __code WLAN_EN =        GPIO(G, 1);
struct Gpio __code WLAN_PWR_EN =    GPIO(A, 3);
struct Gpio __code XLP_OUT =        GPIO(B, 4);
// clang-format on

void gpio_init() {
    // Enable LPC reset on GPD2
    GCR = 0x04;
    // Enable SMBus channel 4
    GCR15 = BIT(4);
    // Set GPF2 and GPF3 to 3.3V
    GCR20 = 0;

    // Set GPIO data
    GPDRA = 0;
    // XLP_OUT, PWR_SW#
    GPDRB = BIT(4) | BIT(3);
    GPDRC = 0;
    // PWR_BTN#, SMI#, SCI#
    GPDRD = BIT(5) | BIT(4) | BIT(3);
    // USB_PWR_EN#
    GPDRE = BIT(3);
    // EC_PECI
    GPDRF = BIT(6);
    // H_PROCHOT#_EC
    GPDRG = BIT(6);
    GPDRH = 0;
    // EC_AMP_EN
    GPDRI = BIT(5);
    // KBC_MUTE#
    GPDRJ = BIT(1);

    // Set GPIO control
    // EC_PWM_PIN_24
    GPCRA0 = GPIO_ALT;
    // KBC_BEEP
    GPCRA1 = GPIO_ALT;
    // CPU_FAN
    GPCRA2 = GPIO_ALT;
    // WLAN_PWR_EN
    GPCRA3 = GPIO_OUT | GPIO_UP;
    // VGA_FAN
    GPCRA4 = GPIO_ALT;
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
    // LAN_WAKEUP#
    GPCRB2 = GPIO_IN | GPIO_UP;
    // PWR_SW#
    GPCRB3 = GPIO_IN;
    // XLP_OUT
    GPCRB4 = GPIO_OUT;
    // SWI#
    GPCRB5 = GPIO_OUT | GPIO_UP;
    // SUSBC_EN#
    GPCRB6 = GPIO_OUT | GPIO_UP;
    // Does not exist
    GPCRB7 = GPIO_IN;
    // ALL_SYS_PWRGD
    GPCRC0 = GPIO_IN;
    // SMC_VGA_THERM
    GPCRC1 = GPIO_ALT;
    // SMD_VGA_THERM
    GPCRC2 = GPIO_ALT;
    // KB-SO16
    GPCRC3 = GPIO_IN;
    // CNVI_DET#
    GPCRC4 = GPIO_IN | GPIO_UP;
    // KB-SO17
    GPCRC5 = GPIO_IN;
    // PM_PWROK
    GPCRC6 = GPIO_OUT;
    // LED_ACIN
    GPCRC7 = GPIO_OUT | GPIO_UP;
    // LED_PWR
    GPCRD0 = GPIO_OUT | GPIO_UP;
    // CCD_EN
    GPCRD1 = GPIO_OUT;
    // BUF_PLT_RST#
    GPCRD2 = GPIO_ALT;
    // SCI#
    GPCRD3 = GPIO_IN;
    // SMI#
    GPCRD4 = GPIO_IN;
    // PWR_BTN#
    GPCRD5 = GPIO_OUT | GPIO_UP;
    // CPU_FANSEN
    GPCRD6 = GPIO_ALT;
    // VGA_FANSEN
    GPCRD7 = GPIO_ALT;
    // SMC_BAT
    GPCRE0 = GPIO_ALT;
    // AC_PRESENT
    GPCRE1 = GPIO_OUT | GPIO_DOWN;
    // PERKB_DET#
    GPCRE2 = GPIO_IN | GPIO_UP;
    // USB_PWR_EN#
    GPCRE3 = GPIO_OUT | GPIO_UP;
    // DD_ON
    GPCRE4 = GPIO_OUT | GPIO_DOWN;
    // EC_RSMRST#
    GPCRE5 = GPIO_OUT;
    // SB_KBCRST#
    GPCRE6 = GPIO_OUT | GPIO_UP;
    // SMD_BAT
    GPCRE7 = GPIO_ALT;
    // 80CLK
    GPCRF0 = GPIO_IN;
    // USB_CHARGE_EN
    GPCRF1 = GPIO_OUT | GPIO_UP;
    // 3IN1
    GPCRF2 = GPIO_IN | GPIO_UP;
    // BT_EN
    GPCRF3 = GPIO_OUT | GPIO_UP;
    // TP_CLK
    GPCRF4 = GPIO_ALT;
    // TP_DATA
    GPCRF5 = GPIO_ALT;
    // EC_PECI
    GPCRF6 = GPIO_ALT;
    // DGPU_PWR_EN
    GPCRF7 = GPIO_IN;
    //TODO NV_POWER_IC_EN
    GPCRG0 = GPIO_OUT | GPIO_UP;
    // WLAN_EN
    GPCRG1 = GPIO_OUT | GPIO_UP;
    // AUTO_LOAD_PWR
    GPCRG2 = GPIO_OUT;
    // ALSPI_CE#
    GPCRG3 = GPIO_ALT;
    // ALSPI_MSI
    GPCRG4 = GPIO_ALT;
    // ALSPI_MSO
    GPCRG5 = GPIO_ALT;
    // H_PROCHOT#_EC
    GPCRG6 = GPIO_OUT | GPIO_UP;
    // ALSPI_SCLK
    GPCRG7 = GPIO_ALT;
    // PM_CLKRUN#
    GPCRH0 = GPIO_ALT;
    // SUSC#_PCH
    GPCRH1 = GPIO_IN;
    // BKL_EN
    GPCRH2 = GPIO_OUT | GPIO_UP;
    // dGPU_GPIO8_OVERT
    GPCRH3 = GPIO_IN | GPIO_UP;
    // PCIE_WAKE#
    GPCRH4 = GPIO_IN;
    // LED_BAT_CHG
    GPCRH5 = GPIO_OUT | GPIO_UP;
    // SUSB#_PCH
    GPCRH6 = GPIO_IN;
    // VCCIO_3P3_PWRGATE
    GPCRH7 = GPIO_IN;
    // BAT_DET
    GPCRI0 = GPIO_ALT;
    // BAT_VOLT
    GPCRI1 = GPIO_ALT;
    // ME_WE
    GPCRI2 = GPIO_OUT;
    // THERM_VOLT
    GPCRI3 = GPIO_ALT;
    // TOTAL_CUR
    GPCRI4 = GPIO_ALT;
    // EC_AMP_EN
    GPCRI5 = GPIO_OUT;
    // SLP_S0#
    GPCRI6 = GPIO_IN;
    // MODEL_ID
    GPCRI7 = GPIO_IN;
    // LED_BAT_FULL
    GPCRJ0 = GPIO_OUT | GPIO_UP;
    // KBC_MUTE#
    GPCRJ1 = GPIO_OUT;
    // NC
    GPCRJ2 = GPIO_IN | GPIO_UP;
    // RGBKB-DET#
    GPCRJ3 = GPIO_IN | GPIO_UP;
    // EC_SLP_SUS#
    GPCRJ4 = GPIO_OUT;
    // VBATT_BOOST#
    GPCRJ5 = GPIO_OUT;
    // SLP_S5#
    GPCRJ6 = GPIO_IN;
    // GC6_FB_EN_PCH
    GPCRJ7 = GPIO_IN;
    // LPC_AD0
    GPCRM0 = GPIO_ALT;
    // LPC_AD1
    GPCRM1 = GPIO_ALT;
    // LPC_AD2
    GPCRM2 = GPIO_ALT;
    // LPC_AD3
    GPCRM3 = GPIO_ALT;
    // PCLK_KBC
    GPCRM4 = GPIO_ALT;
    // LPC_FRAME#
    GPCRM5 = GPIO_ALT;
    // SERIRQ
    GPCRM6 = GPIO_ALT;
}
