// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/macro.h>

// clang-format off
struct Gpio __code ACIN_N =         GPIO(B, 0);
struct Gpio __code AC_PRESENT =     GPIO(E, 1);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(H, 2);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2);
struct Gpio __code CCD_EN =         GPIO(D, 1);
struct Gpio __code CPU_C10_GATE_N = GPIO(F, 7);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code EC_EN =          GPIO(B, 6); // renamed to SUSBC_EC#
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code LED_ACIN =       GPIO(C, 7);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code ME_WE =          GPIO(I, 2);
struct Gpio __code PCH_DPWROK_EC =  GPIO(A, 7);
struct Gpio __code PCH_PWROK_EC =   GPIO(A, 6);
struct Gpio __code PD_EN =          GPIO(H, 4); // renamed to PD_POWER_EN
struct Gpio __code PM_PWROK =       GPIO(C, 6);
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code SLP_S0_N =       GPIO(J, 0);
struct Gpio __code SLP_SUS_N =      GPIO(J, 3);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code SWI_N =          GPIO(B, 5);
struct Gpio __code VA_EC_EN =       GPIO(J, 4);
struct Gpio __code WLAN_PWR_EN =    GPIO(A, 3);
struct Gpio __code XLP_OUT =        GPIO(B, 4);
// clang-format on

void gpio_init(void) {
    // Enable LPC reset on GPD2
    GCR = 0b10 << 1;
    // Disable UARTs
    GCR6 = 0;
    // PWRSW WDT 2 Enable 1
    GCR8 = BIT(4);
    // PWRSW WDT 2 Enable 2
    GCR9 = BIT(5);
    // PWRSW counter 12 seconds
    GCR10 = BIT(1);
    // Enable SMBus channel 4
    GCR15 = BIT(4);
    // Set GPD2 to 1.8V
    GCR19 = BIT(0);
    // Set GPF2 and GPF3 to 3.3V
    GCR20 = 0;
    // Set GPM6 power domain to VCC
    GCR23 = BIT(0);

    //TODO: what do these do?
    GCR1 = 0;
    GCR2 = 0;
    GCR21 = 0;
    GCR22 = BIT(7);

    // Set GPIO data
    // PCH_DPWROK_EC
    GPDRA = BIT(7);
    // XLP_OUT
    GPDRB = BIT(4);
    GPDRC = 0;
    // PWR_BTN#, SMI#
    GPDRD = BIT(5) | BIT(4);
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
    GPOTA = 0;
    GPOTB = 0;
    GPOTD = 0;
    GPOTE = 0;
    GPOTF = 0;
    GPOTH = 0;
    GPOTJ = 0;

    // Set GPIO control
    // Not connected
    GPCRA0 = GPIO_IN;
    // KBC_BEEP
    GPCRA1 = GPIO_ALT;
    // CPU_FAN
    GPCRA2 = GPIO_ALT;
    // WLAN_PWR_EN
    GPCRA3 = GPIO_OUT;
    // Not connected
    GPCRA4 = GPIO_IN;
    // Not connected
    GPCRA5 = GPIO_IN;
    // PCH_PWROK_EC
    GPCRA6 = GPIO_OUT;
    // PCH_DPWROK_EC
    GPCRA7 = GPIO_OUT;
    // AC_IN#
    GPCRB0 = GPIO_IN | GPIO_UP;
    // LID_SW#
    GPCRB1 = GPIO_IN | GPIO_UP;
    // SLP_WLAN#
    GPCRB2 = GPIO_IN;
    // PWR_SW#
    GPCRB3 = GPIO_IN;
    // XLP_OUT
    GPCRB4 = GPIO_OUT;
    // SWI#
    GPCRB5 = GPIO_IN;
    // SUSBC_EC#
    GPCRB6 = GPIO_OUT | GPIO_UP;
    // Does not exist
    GPCRB7 = GPIO_IN;
    // ALL_SYS_PWRGD
    GPCRC0 = GPIO_IN;
    // SMB_CLK_EC
    GPCRC1 = GPIO_ALT | GPIO_UP;
    // SMB_DATA_EC
    GPCRC2 = GPIO_ALT | GPIO_UP;
    // PCIE_WAKE#
    GPCRC3 = GPIO_IN;
    // CNVI_DET#
    GPCRC4 = GPIO_IN | GPIO_UP;
    // 3G_PWR_EN
    GPCRC5 = GPIO_OUT | GPIO_UP;
    // PM_PWROK
    GPCRC6 = GPIO_OUT;
    // LED_ACIN
    GPCRC7 = GPIO_OUT | GPIO_UP;
    // LED_PWR
    GPCRD0 = GPIO_OUT;
    // CCD_EN
    GPCRD1 = GPIO_OUT | GPIO_UP;
    // ESPI_RESET#
    GPCRD2 = GPIO_ALT;
    // SCI#
    GPCRD3 = GPIO_IN;
    // SMI#
    GPCRD4 = GPIO_OUT;
    // PWR_BTN#
    GPCRD5 = GPIO_OUT;
    // CPU_FANSEN
    GPCRD6 = GPIO_ALT | GPIO_DOWN;
    // SINK_CTRL_EC
    GPCRD7 = GPIO_IN;
    // SMC_BAT
    GPCRE0 = GPIO_ALT | GPIO_UP;
    // AC_PRESENT
    GPCRE1 = GPIO_OUT;
    // Not connected?
    GPCRE2 = GPIO_IN;
    // USB_PWR_EN
    GPCRE3 = GPIO_OUT | GPIO_UP;
    // DD_ON
    GPCRE4 = GPIO_OUT;
    // EC_RSMRST#
    GPCRE5 = GPIO_OUT;
    // SB_KBCRST#
    GPCRE6 = GPIO_IN;
    // SMD_BAT
    GPCRE7 = GPIO_ALT | GPIO_UP;
    // 80CLK
    GPCRF0 = GPIO_IN | GPIO_UP;
    // USB_CHARGE_EN
    GPCRF1 = GPIO_OUT;
    // 3IN1
    GPCRF2 = GPIO_IN | GPIO_UP;
    // EC_BT_EN
    GPCRF3 = GPIO_IN;
    // TP_CLK
    GPCRF4 = GPIO_ALT | GPIO_UP;
    // TP_DATA
    GPCRF5 = GPIO_ALT | GPIO_UP;
    // H_PECI
    GPCRF6 = GPIO_ALT;
    // CPU_C10_GATE#
    GPCRF7 = GPIO_IN;
    // VCCIN_AUX_PG
    GPCRG0 = GPIO_IN;
    // WLAN_EN
    GPCRG1 = GPIO_IN;
    // Pull up to VDD3
    GPCRG2 = GPIO_IN;
    // ALSPI_CE#
    GPCRG3 = GPIO_ALT;
    // ALSPI_MSI
    GPCRG4 = GPIO_ALT;
    // ALSPI_MSO
    GPCRG5 = GPIO_ALT;
    // H_PROCHOT_EC
    GPCRG6 = GPIO_OUT;
    // ALSPI_SCLK
    GPCRG7 = GPIO_ALT;
    // EC_CLKRUN#
    GPCRH0 = GPIO_IN;
    // SUSC#_PCH
    GPCRH1 = GPIO_IN;
    // BKL_EN
    GPCRH2 = GPIO_OUT | GPIO_UP;
    // 3G_EN
    GPCRH3 = GPIO_OUT | GPIO_UP;
    // PD_POWER_EN
    GPCRH4 = GPIO_OUT;
    // JACK_IN#_EC
    GPCRH5 = GPIO_IN;
    // SUSB#_PCH
    GPCRH6 = GPIO_IN;
    // ACE_I2C_IRQ2Z
    GPCRH7 = GPIO_IN;
    // BAT_DET
    GPCRI0 = GPIO_ALT;
    // BAT_VOLT
    GPCRI1 = GPIO_ALT;
    // ME_WE
    GPCRI2 = GPIO_OUT;
    // THERM_VOLT_CPU
    GPCRI3 = GPIO_ALT;
    // TOTAL_CUR
    GPCRI4 = GPIO_ALT;
    // Not connected
    GPCRI5 = GPIO_IN;
    // PM_BATLOW#
    GPCRI6 = GPIO_IN;
    // MODEL_ID
    GPCRI7 = GPIO_ALT;
    // SLP_S0#
    GPCRJ0 = GPIO_IN;
    // KBC_MUTE#
    GPCRJ1 = GPIO_OUT;
    // KBLIGHT_ADJ
    GPCRJ2 = GPIO_ALT;
    // SLP_SUS#
    GPCRJ3 = GPIO_IN;
    // VA_EC_EN
    GPCRJ4 = GPIO_OUT;
    // VBATT_BOOST#
    GPCRJ5 = GPIO_IN;
    // EC_GPIO
    GPCRJ6 = GPIO_OUT | GPIO_UP;
    // LEDKB_DET#
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
