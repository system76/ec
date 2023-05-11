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
struct Gpio __code CPU_C10_GATE_N = GPIO(J, 2);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code DGPU_PWR_EN =    GPIO(H, 3);
#if CONFIG_MUX_COMBO
    struct Gpio __code MUX_CTRL_BIOS =  GPIO(I, 2);
#endif
struct Gpio __code EC_EN =          GPIO(B, 6); // renamed to SUSBC_EC#
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
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);//PWR_BTN to PCH
struct Gpio __code PWR_SW_N =       GPIO(B, 3);//PWR_BTN to switch
struct Gpio __code SLP_SUS_N =      GPIO(D, 4);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code VA_EC_EN =       GPIO(J, 4); // renamed to EC_SLP_SUS#
struct Gpio __code WLAN_EN =        GPIO(G, 1);
struct Gpio __code WLAN_PWR_EN =    GPIO(H, 4);
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
    // Set GPH0 to 1.8V
    GCR21 = BIT(2);

    // Set GPIO data
    // DDS_EC_PWM
    GPDRA = BIT(3);
    // SWI#, XLP_OUT, PWR_SW#
    GPDRB = BIT(5) | BIT(4) | BIT(3);
    GPDRC = 0;
    // PWR_BTN#
    GPDRD = BIT(5);
    GPDRE = 0;
    // BL_PWM_EN_EC, PCH_DPWROK_EC
    GPDRF = BIT(7) | BIT(3);
    // H_PROCHOT#_EC
    GPDRG = BIT(6);
    GPDRH = 0;
    // PLVDD_RST_EC, EC_AMP_EN
    GPDRI = BIT(6) | BIT(5);
    GPDRJ = 0;

    // Set GPIO control
    // EC_PWM_LEDKB_P
    GPCRA0 = GPIO_ALT;
    // KBC_BEEP
    GPCRA1 = GPIO_ALT;
    // CPU_FAN_PWM
    GPCRA2 = GPIO_ALT;
    // DDS_EC_PWM
    GPCRA3 = GPIO_IN;
    // VGA_FAN_PWM1
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
    // SUSBC_EC#
    GPCRB6 = GPIO_OUT | GPIO_UP;
    // Doesn't exist
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
    // PCH_PWROK_EC
    GPCRC6 = GPIO_OUT;
    // BKL_EN
    GPCRC7 = GPIO_OUT | GPIO_UP;
    // LED_PWR
    GPCRD0 = GPIO_OUT | GPIO_UP;
    // CCD_EN
    GPCRD1 = GPIO_OUT | GPIO_UP;
    // ESPI_RESET#
    GPCRD2 = GPIO_ALT;
    // SLP_A#
    GPCRD3 = GPIO_IN;
    // SLP_SUS#
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
    GPCRE1 = GPIO_OUT | GPIO_UP;
    // PERKB_DET#
    GPCRE2 = GPIO_IN | GPIO_UP;
    // USB_PWR_EN#
    GPCRE3 = GPIO_OUT;
    // DD_ON
    GPCRE4 = GPIO_OUT | GPIO_DOWN;
    // EC_RSMRST#
    GPCRE5 = GPIO_OUT;
    // SB_KBCRST#
    GPCRE6 = GPIO_IN;
    // SMD_BAT
    GPCRE7 = GPIO_ALT;
    // 80CLK
    GPCRF0 = GPIO_IN;
    // USB_CHARGE_EN
    GPCRF1 = GPIO_OUT | GPIO_UP;
    // 3IN1
    GPCRF2 = GPIO_IN | GPIO_UP;
    // PCH_DPWROK_EC
    GPCRF3 = GPIO_OUT;
    // TP_CLK
    GPCRF4 = GPIO_ALT;
    // TP_DATA
    GPCRF5 = GPIO_ALT;
    // H_PECI
    GPCRF6 = GPIO_ALT;
    // BL_PWM_EN_EC
    GPCRF7 = GPIO_OUT;
    // dGPU_OVERT_EC
    GPCRG0 = GPIO_IN | GPIO_UP;
    // WLAN_EN
    GPCRG1 = GPIO_OUT | GPIO_UP;
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
    // DGPU_PWR_EN
    GPCRH3 = GPIO_IN;
    // WLAN_PWR_EN
    GPCRH4 = GPIO_OUT;
    // LED_BAT_CHG
    GPCRH5 = GPIO_OUT;
    // SUSB#_PCH
    GPCRH6 = GPIO_IN;
    // ACE_I2C_IRQ2Z_EC
    GPCRH7 = GPIO_OUT | GPIO_UP;
    // BAT_DET
    GPCRI0 = GPIO_ALT;
    // BAT_VOLT
    GPCRI1 = GPIO_ALT;
    // MUX_CTRL_BIOS
    GPCRI2 = GPIO_OUT;
    // THERM_VOLT
    GPCRI3 = GPIO_ALT;
    // TOTAL_CUR
    GPCRI4 = GPIO_ALT;
    // EC_AMP_EN
    GPCRI5 = GPIO_OUT;
    // PLVDD_RST_EC
    GPCRI6 = GPIO_OUT;
    // MODEL_ID
    GPCRI7 = GPIO_IN;
    // LED_BAT_FULL
    GPCRJ0 = GPIO_OUT | GPIO_UP;
    // KBC_MUTE#
    GPCRJ1 = GPIO_IN;
    // CPU_C10_GATE#
    GPCRJ2 = GPIO_IN;
    // GC6_FB_EN_PCH
    GPCRJ3 = GPIO_IN;
    // EC_SLP_SUS#
    GPCRJ4 = GPIO_OUT;
    // VBATT_BOOST#
    GPCRJ5 = GPIO_OUT;
    // EC_GPIO
    GPCRJ6 = GPIO_OUT;
    // RGBKB-DET#
    GPCRJ7 = GPIO_IN | GPIO_UP;
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
    // ESPI_CS#_EC
    GPCRM5 = GPIO_ALT;
    // SERIRQ_ESPI_ALERT0
    GPCRM6 = GPIO_IN | GPIO_UP;
}
