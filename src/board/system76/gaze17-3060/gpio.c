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
struct Gpio __code CPU_C10_GATE_N = GPIO(J, 2);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code DGPU_PWR_EN =    GPIO(H, 4);
struct Gpio __code EC_EN =          GPIO(B, 6); // renamed to SUSBC_EN#
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code GC6_FB_EN =      GPIO(J, 3);
struct Gpio __code LAN_WAKEUP_N =   GPIO(B, 2);
struct Gpio __code LED_ACIN =       GPIO(H, 2);
struct Gpio __code LED_BAT_CHG =    GPIO(H, 5);
struct Gpio __code LED_BAT_FULL =   GPIO(J, 0);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code ME_WE =          GPIO(H, 0);
struct Gpio __code PCH_DPWROK_EC =  GPIO(F, 3);
struct Gpio __code PCH_PWROK_EC =   GPIO(C, 6);
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code RGBKB_DET_N =    GPIO(E, 2);
struct Gpio __code SLP_S0_N =       GPIO(J, 2); // XXX: Really CPU_C10_GATE#
struct Gpio __code SLP_SUS_N =      GPIO(J, 7);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code VA_EC_EN =       GPIO(J, 4);
struct Gpio __code WLAN_EN =        GPIO(G, 1);
struct Gpio __code WLAN_PWR_EN =    GPIO(D, 3);
struct Gpio __code XLP_OUT =        GPIO(B, 4);
// clang-format on

void gpio_init() {
    // PWRSW WDT 2 Enable 2
    GCR9 = BIT(5);
    // PWRSW WDT 2 Enable 1
    GCR8 = BIT(4);

    // Enable LPC reset on GPD2
    GCR = 0x04;
    // Disable UARTs
    GCR6 = 0;
    // Enable SMBus channel 4
    GCR15 = BIT(4);
    // Set GPD2 to 1.8V
    GCR19 = BIT(0);
    // Set GPF2 and GPF3 to 3.3V
    GCR20 = 0;

    // Set GPIO data
    GPDRA = 0;
    // XLP_OUT
    GPDRB = BIT(4);
    GPDRC = 0;
    GPDRD = 0;
    // USB_PWR_EN
    GPDRE = BIT(3);
    GPDRF = 0;
    // H_PROCHOT_EC
    GPDRG = BIT(6);
    GPDRH = 0;
    GPDRI = 0;
    // KBC_MUTE#
    GPDRJ = BIT(1);
    GPOTA = 0;
    GPOTB = 0;
    GPOTD = 0;
    GPOTE = 0;
    GPOTF = 0;
    GPOTH = 0;
    GPOTJ = 0;

    // Set GPIO control
    // EC_PWM_PIN_24
    GPCRA0 = GPIO_ALT;
    // KBC_BEEP
    GPCRA1 = GPIO_ALT;
    // CPU_FAN
    GPCRA2 = GPIO_ALT;
    // DDS_EC_PWM
    GPCRA3 = GPIO_IN | GPIO_UP;
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
    // BL_PWM_EN_EC
    GPCRB5 = GPIO_IN | GPIO_UP;
    // SUSBC_EC#
    GPCRB6 = GPIO_OUT | GPIO_UP;
    // Not connected
    GPCRB7 = GPIO_IN;
    // ALL_SYS_PWRGD
    GPCRC0 = GPIO_IN;
    // SMC_VGA_THERM
    GPCRC1 = GPIO_ALT | GPIO_UP;
    // SMD_VGA_THERM
    GPCRC2 = GPIO_ALT | GPIO_UP;
    // KB_SO16
    GPCRC3 = GPIO_IN;
    // CNVI_DET#
    GPCRC4 = GPIO_IN | GPIO_UP;
    // KB_SO17
    GPCRC5 = GPIO_IN;
    // PCH_PWROK_EC
    GPCRC6 = GPIO_OUT;
    // BKL_EN
    GPCRC7 = GPIO_OUT | GPIO_UP;
    // LED_PWR
    GPCRD0 = GPIO_OUT | GPIO_UP;
    // CCD_EN
    GPCRD1 = GPIO_OUT;
    // ESPI_RESET#
    GPCRD2 = GPIO_ALT;
    // WLAN_PWR_EN
    GPCRD3 = GPIO_OUT;
    // PLVDD_RST_EC
    GPCRD4 = GPIO_IN | GPIO_UP;
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
    GPCRE3 = GPIO_OUT;
    // DD_ON
    GPCRE4 = GPIO_OUT | GPIO_DOWN;
    // EC_RSMRST#
    GPCRE5 = GPIO_OUT;
    // SB_KBCRST#
    GPCRE6 = GPIO_IN;
    // SMD_BAT
    GPCRE7 = GPIO_ALT | GPIO_UP;
    // 80CLK
    GPCRF0 = GPIO_IN;
    // USB_CHARGE_EN
    GPCRF1 = GPIO_OUT | GPIO_UP;
    // 3IN1
    GPCRF2 = GPIO_IN | GPIO_UP;
    // PCH_DPWROK_EC
    GPCRF3 = GPIO_OUT;
    // TP_CLK
    GPCRF4 = GPIO_ALT | GPIO_UP;
    // TP_DATA
    GPCRF5 = GPIO_ALT | GPIO_UP;
    // H_PECI
    GPCRF6 = GPIO_ALT;
    // CC_EN: TODO!
    GPCRF7 = GPIO_IN | GPIO_UP;
    // OVERT#_EC
    GPCRG0 = GPIO_IN;
    // WLAN_EN
    GPCRG1 = GPIO_OUT;
    // AUTO_LOAD_PWR
    GPCRG2 = GPIO_IN;
    // ALSPI_CE#
    GPCRG3 = GPIO_ALT;
    // ALSPI_MSI
    GPCRG4 = GPIO_ALT;
    // ALSPI_MSO
    GPCRG5 = GPIO_ALT;
    // H_PROCHOT_EC
    GPCRG6 = GPIO_OUT | GPIO_UP;
    // ALSPI_SCLK
    GPCRG7 = GPIO_ALT;
    // ME_WE
    GPCRH0 = GPIO_OUT;
    // SUSC#_PCH
    GPCRH1 = GPIO_IN;
    // LED_ACIN
    GPCRH2 = GPIO_OUT | GPIO_UP;
    // MUX_CTRL_BIOS
    GPCRH3 = GPIO_IN | GPIO_DOWN;
    // DGPU_PWR_EN
    GPCRH4 = GPIO_IN;
    // LED_BAT_CHG
    GPCRH5 = GPIO_OUT | GPIO_UP;
    // SUSB#_PCH
    GPCRH6 = GPIO_IN;
    // d_GPIO9_ALERT_FAN
    GPCRH7 = GPIO_IN;
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
    GPCRI7 = GPIO_IN;
    // LED_BAT_FULL
    GPCRJ0 = GPIO_OUT | GPIO_UP;
    // KBC_MUTE#
    GPCRJ1 = GPIO_OUT;
    // CPU_C10_GATE#
    GPCRJ2 = GPIO_IN;
    // GC6_FB_EN_PCH
    GPCRJ3 = GPIO_IN;
    // VA_EC_EN
    GPCRJ4 = GPIO_OUT;
    // VBATT_BOOST#
    GPCRJ5 = GPIO_OUT;
    // EC_GPIO
    GPCRJ6 = GPIO_OUT;
    // SLP_SUS#
    GPCRJ7 = GPIO_IN;
    // ESPI_IO0_EC
    GPCRM0 = GPIO_ALT;
    // ESPI_IO1_EC
    GPCRM1 = GPIO_ALT;
    // ESPI_IO2_EC
    GPCRM2 = GPIO_ALT;
    // ESPI_IO3_EC
    GPCRM3 = GPIO_ALT;
    // ESPI_CLK_EC
    GPCRM4 = GPIO_ALT;
    // ESPI_CS_EC#
    GPCRM5 = GPIO_ALT;
    // ESPI_ALRT0#
    GPCRM6 = GPIO_IN | GPIO_UP;
}
