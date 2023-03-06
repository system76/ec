// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/macro.h>

// clang-format off
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
struct Gpio __code SLP_SUS_N =      GPIO(J, 4);
struct Gpio __code VA_EC_EN =       GPIO(H, 7);
struct Gpio __code WLAN_EN =        GPIO(G, 1);
struct Gpio __code WLAN_PWR_EN =    GPIO(A, 3);
struct Gpio __code XLP_OUT =        GPIO(B, 4);
// clang-format on

void gpio_init() {
    // Not documented
    //GCR22 = BIT(7);
    GCR10 = 0x02;
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
    // Set GPF2 and GPF3 to 3.3V
    GCR20 = 0;
    // Set GPD2 to 1.8V
    GCR19 = BIT(0);
    // Not documented
    //GCR22 = BIT(7);
    // Set GPM6 power domain to VCC
    GCR23 = BIT(0);

    GPDRA = 0;
    // XLP_OUT
    GPDRB = BIT(4);
    GPDRC = 0;
    GPDRD = 0;
    // USB_PWR_EN
    GPDRE = BIT(3);
    // CC_EN
    GPDRF = BIT(7);
    // H_PROCHOT#_EC
    GPDRG = BIT(6);
    // PCH_DPWROK_EC
    GPDRH = BIT(4);
    GPDRI = 0;
    GPDRJ = 0;

    // Set GPIO control

    // EC_PWM_PIN_24
    GPCRA0 = GPIO_IN;
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
    GPCRB5 = GPIO_OUT;
    // SUSBC_EC#
    GPCRB6 = GPIO_OUT | GPIO_UP;
    // Not connected
    GPCRB7 = GPIO_IN | GPIO_UP;

    // ALL_SYS_PWRGD
    GPCRC0 = GPIO_IN;
    // SMC_VGA_THERM
    GPCRC1 = GPIO_ALT | GPIO_UP;
    // SMD_VGA_THERM
    GPCRC2 = GPIO_ALT | GPIO_UP;
    // KB_SO16
    GPCRC3 = GPIO_ALT | GPIO_UP;
    // CNVI_DET#
    GPCRC4 = GPIO_IN | GPIO_UP;
    // KB_SO17
    GPCRC5 = GPIO_ALT | GPIO_UP;
    // CPU_C10_GATE#
    GPCRC6 = GPIO_IN;
    // BKL_EN
    GPCRC7 = GPIO_OUT | GPIO_UP;

    // LED_PWR
    GPCRD0 = GPIO_OUT | GPIO_UP;
    // CCD_EN
    GPCRD1 = GPIO_OUT | GPIO_UP;
    // ESPI_RESET_N
    GPCRD2 = GPIO_ALT;
    // SCI#
    GPCRD3 = GPIO_OUT;
    // SMI#
    GPCRD4 = GPIO_OUT;
    // PWR_BTN#
    GPCRD5 = GPIO_OUT | GPIO_UP;
    // CPU_FANSEN
    GPCRD6 = GPIO_ALT;
    // VGA_FANSEN
    GPCRD7 = GPIO_ALT;

    // SMC_BAT
    GPCRE0 = GPIO_ALT | GPIO_UP;
    // AC_PRESENT
    GPCRE1 = GPIO_OUT | GPIO_UP;
    // RGBKB-DET#
    GPCRE2 = GPIO_IN | GPIO_UP;
    // USB_PWR_EN
    GPCRE3 = GPIO_OUT | GPIO_UP;
    // DD_ON
    GPCRE4 = GPIO_OUT | GPIO_DOWN;
    // EC_RSMRST#
    GPCRE5 = GPIO_OUT;
    // ME_WE
    GPCRE6 = GPIO_OUT;
    // SMD_BAT
    GPCRE7 = GPIO_ALT | GPIO_UP;

    // 80CLK
    GPCRF0 = GPIO_IN;
    // USB_CHARGE_EN
    GPCRF1 = GPIO_OUT | GPIO_UP;
    // 3IN1
    GPCRF2 = GPIO_IN | GPIO_UP;
    // PCH_PWROK_EC
    GPCRF3 = GPIO_OUT;
    // TP_CLK
    GPCRF4 = GPIO_OUT | GPIO_DOWN;
    // TP_DATA
    GPCRF5 = GPIO_OUT | GPIO_DOWN;
    // H_PECI
    GPCRF6 = GPIO_ALT;
    // CC_EN
    GPCRF7 = GPIO_OUT | GPIO_UP;

    // OVERT#_EC
    GPCRG0 = GPIO_IN;
    // WLAN_EN
    GPCRG1 = GPIO_OUT | GPIO_UP;
    // Not connected
    GPCRG2 = GPIO_IN;
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
    GPCRH0 = GPIO_IN;
    // SUSC#_PCH
    GPCRH1 = GPIO_IN;
    // LED_ACIN
    GPCRH2 = GPIO_OUT | GPIO_UP;
    // LIGHT_KB_DET#
    GPCRH3 = GPIO_IN | GPIO_UP;
    // PCH_DPWROK_EC
    GPCRH4 = GPIO_OUT;
    // LED_BAT_CHG
    GPCRH5 = GPIO_OUT | GPIO_UP;
    // SUSB#_PCH
    GPCRH6 = GPIO_IN;
    // VA_EC_EN
    GPCRH7 = GPIO_OUT | GPIO_UP;

    // BAT_DET
    GPCRI0 = GPIO_ALT;
    // BAT_VOLT
    GPCRI1 = GPIO_ALT;
    // THERM_VOLT2
    GPCRI2 = GPIO_ALT;
    // THERM_VOLT
    GPCRI3 = GPIO_ALT;
    // TOTAL_CUR
    GPCRI4 = GPIO_ALT;
    // CC1_DET
    GPCRI5 = GPIO_ALT;
    // CC2_DET
    GPCRI6 = GPIO_ALT;
    // MODEL_ID
    GPCRI7 = GPIO_ALT;

    // LED_BAT_FULL
    GPCRJ0 = GPIO_OUT | GPIO_UP;
    // KBC_MUTE#
    GPCRJ1 = GPIO_IN;
    // DGPU_PWR_EN
    GPCRJ2 = GPIO_IN;
    // GC6_FB_EN_PCH
    GPCRJ3 = GPIO_IN;
    // SLP_SUS#
    GPCRJ4 = GPIO_IN;
    // VBATT_BOOST#
    GPCRJ5 = GPIO_OUT;
    // EC_GPIO
    GPCRJ6 = GPIO_OUT | GPIO_UP;
    // SLP_S0#
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
