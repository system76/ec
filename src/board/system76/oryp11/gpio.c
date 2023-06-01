// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/macro.h>

// clang-format off
struct Gpio __code ACIN_N =         GPIO(B, 0);
struct Gpio __code AC_PRESENT =     GPIO(E, 1);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(C, 7);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2); // renamed to ESPI_RESET#
struct Gpio __code CCD_EN =         GPIO(D, 1);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code DGPU_PWR_EN =    GPIO(H, 4);
struct Gpio __code EC_EN =          GPIO(B, 6); // renamed to SUSBC_EC#
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code GC6_FB_EN =      GPIO(J, 3);
struct Gpio __code JACK_IN_N =      GPIO(G, 1);
struct Gpio __code LAN_WAKEUP_N =   GPIO(B, 2);
struct Gpio __code LED_ACIN =       GPIO(H, 2);
struct Gpio __code LED_BAT_CHG =    GPIO(H, 5);
struct Gpio __code LED_BAT_FULL =   GPIO(J, 0);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code ME_WE =          GPIO(H, 0);
struct Gpio __code PCH_DPWROK_EC =  GPIO(F, 3);
struct Gpio __code PCH_PWROK_EC =   GPIO(C, 6); // renamed to SYS_PWROK_EC
struct Gpio __code PD_EN =          GPIO(F, 6); // renamed to PD_POWER_EN
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code SINK_CTRL =      GPIO(H, 1);
struct Gpio __code SLP_SUS_N =      GPIO(H, 7);
struct Gpio __code VA_EC_EN =       GPIO(J, 4);
struct Gpio __code WLAN_PWR_EN =    GPIO(D, 3);
struct Gpio __code XLP_OUT =        GPIO(B, 4);
// clang-format on

void gpio_init() {
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

    //TODO: what do these do?
    GCR1 = 0;
    GCR2 = 0;
    GCR8 = 0x10;
    GCR9 = 0x20;
    GCR10 = 0x02;
    GCR21 = 0;
    GCR22 = 0x80;
    GCR22 = 0x80;
    GCR23 = 0x01;

    // Set GPIO data
    GPDRA = 0;
    // BL_PWM_EN_EC, XLP_OUT
    GPDRB = BIT(5) | BIT(4);
    GPDRC = 0;
    // PLVDD_RST_EC
    GPDRD = BIT(4);
    // USB_PWR_EN
    GPDRE = BIT(3);
    // CC_EN, PCH_DPWROK_EC
    GPDRF = BIT(7) | BIT(3);
    // H_PROCHOT_EC
    GPDRG = BIT(6);
    GPDRH = 0;
    // LAN_PWR_EN
    GPDRI = BIT(2);
    // EC_AMP_EN
    GPDRJ = BIT(2);
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
    GPCRA3 = GPIO_IN;
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
    GPCRB2 = GPIO_IN;
    // PWR_SW#
    GPCRB3 = GPIO_IN;
    // XLP_OUT
    GPCRB4 = GPIO_OUT;
    // BL_PWM_EN_EC
    GPCRB5 = GPIO_OUT;
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
    // SYS_PWROK_EC
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
    GPCRE3 = GPIO_OUT;
    // DD_ON
    GPCRE4 = GPIO_OUT | GPIO_DOWN;
    // EC_RSMRST#
    GPCRE5 = GPIO_OUT;
    // ACE_I2C_IRQ2Z
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
    // PD_POWER_EN
    GPCRF6 = GPIO_OUT;
    // CC_EN
    GPCRF7 = GPIO_OUT | GPIO_UP;

    // dGPU_OVERT_EC
    GPCRG0 = GPIO_IN;
    // JACK_IN#_EC
    GPCRG1 = GPIO_IN;
    // AUTO_LOAD_PWR
    GPCRG2 = GPIO_IN;
    // ALSPI_CE#_L
    GPCRG3 = GPIO_ALT;
    // ALSPI_MSI_L
    GPCRG4 = GPIO_ALT;
    // ALSPI_MSO_L
    GPCRG5 = GPIO_ALT;
    // H_PROCHOT_EC
    GPCRG6 = GPIO_OUT | GPIO_UP;
    // ALSPI_SCLK_L
    GPCRG7 = GPIO_ALT;

    // ME_WE
    GPCRH0 = GPIO_OUT;
    // SINK_CTRL
    GPCRH1 = GPIO_IN;
    // LED_ACIN
    GPCRH2 = GPIO_OUT | GPIO_UP;
    // MUX_CTRL_BIOS
    GPCRH3 = GPIO_OUT;
    // DGPU_PWR_EN
    GPCRH4 = GPIO_IN;
    // LED_BAT_CHG
    GPCRH5 = GPIO_OUT | GPIO_UP;
    // d_GPIO9_ALERT_FAN
    GPCRH6 = GPIO_IN;
    // SLP_SUS#
    GPCRH7 = GPIO_IN;

    // BAT_DET
    GPCRI0 = GPIO_ALT;
    // BAT_VOLT
    GPCRI1 = GPIO_ALT;
    // LAN_PWR_EN
    GPCRI2 = GPIO_OUT;
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
    GPCRJ1 = GPIO_IN;
    // EC_AMP_EN
    GPCRJ2 = GPIO_OUT;
    // GC6_FB_EN_PCH
    GPCRJ3 = GPIO_IN;
    // VA_EC_EN
    GPCRJ4 = GPIO_OUT;
    // VBATT_BOOST#
    GPCRJ5 = GPIO_IN;
    // EC_GPIO
    GPCRJ6 = GPIO_IN;
    // PERKB_DET#
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
    // SERIRQ_ESPI_ALERT0
    GPCRM6 = GPIO_IN | GPIO_UP | GPIO_DOWN;
}
