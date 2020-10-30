// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/debug.h>
#include <common/macro.h>

struct Gpio __code ACIN_N =         GPIO(B, 0);
struct Gpio __code AC_PRESENT =     GPIO(E, 1);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(H, 2);
struct Gpio __code BT_EN =          GPIO(F, 3);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2);
struct Gpio __code CCD_EN =         GPIO(D, 1);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code EC_EN =          GPIO(J, 6);
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code LED_ACIN =       GPIO(C, 7);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code PCH_DPWROK_EC =  GPIO(A, 7);
struct Gpio __code PCH_PWROK_EC =   GPIO(A, 6);
struct Gpio __code PM_CLKRUN_N =    GPIO(H, 0);
struct Gpio __code PM_PWROK =       GPIO(C, 6);
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code SB_KBCRST_N =    GPIO(E, 6);
struct Gpio __code SCI_N =          GPIO(D, 3);
struct Gpio __code SLP_SUS_N =      GPIO(J, 3);
struct Gpio __code SMI_N =          GPIO(D, 4);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code SWI_N =          GPIO(B, 5);
struct Gpio __code USB_PWR_EN_N =   GPIO(E, 3);
struct Gpio __code VA_EC_EN =       GPIO(J, 4);
struct Gpio __code VR_ON =          GPIO(H, 4);
struct Gpio __code WLAN_EN =        GPIO(G, 1);
struct Gpio __code WLAN_PWR_EN =    GPIO(A, 3);
struct Gpio __code XLP_OUT =        GPIO(B, 4);

void gpio_init() {
    // Enable LPC reset on GPD2
    GCR = 0x04;
    // Set GPF2 and GPF3 to 3.3V
    GCR20 = 0;
    // Enable SMBus channel 4
    GCR15 = BIT(4);
    // Set GPD2 to 1.8V
    GCR19 = BIT(0);
    // Set GPH0 to 1.8V
    GCR21 = BIT(2);

    // Set GPIO data
    // PCH_DPWROK_EC, NC
    GPDRA = BIT(7) | BIT(5);
    // XLP_OUT, PWR_SW#
    GPDRB = BIT(4) | BIT(3);
    GPDRC = 0;
    // PWR_BTN#
    GPDRD = BIT(5);
    // USB_PWR_EN#
    GPDRE = BIT(3);
    // H_PECI
    GPDRF = BIT(6);
    GPDRG = 0;
    // ACE_I2C_IRQ2Z
    GPDRH = BIT(7);
    GPDRI = 0;
    // KBC_MUTE#
    GPDRJ = BIT(1);
    GPDRM = 0;

    // Set GPIO control
    // AC/BATL#
    GPCRA0 = GPIO_IN;
    // KBC_BEEP
    GPCRA1 = GPIO_ALT;
    // CPU_FAN
    GPCRA2 = GPIO_ALT;
    // WLAN_PWR_EN
    GPCRA3 = GPIO_OUT;
    // NC
    GPCRA4 = GPIO_IN;
    // NC
    GPCRA5 = GPIO_OUT | GPIO_UP;
    // PCH_PWROK_EC
    GPCRA6 = GPIO_OUT | GPIO_UP;
    // PCH_DPWROK_EC
    GPCRA7 = GPIO_OUT | GPIO_UP;
    // AC_IN#
    GPCRB0 = GPIO_IN | GPIO_UP;
    // LID_SW#
    GPCRB1 = GPIO_IN | GPIO_UP;
    // PCH_SLP_WLAN#_R
    GPCRB2 = GPIO_IN | GPIO_UP;
    // PWR_SW#
    GPCRB3 = GPIO_IN;
    // XLP_OUT
    GPCRB4 = GPIO_OUT;
    // SWI#
    GPCRB5 = GPIO_OUT | GPIO_UP;
    // EC_FORCE_POWER
    GPCRB6 = GPIO_IN;
    // NC
    GPCRB7 = GPIO_IN;
    // ALL_SYS_PWRGD
    GPCRC0 = GPIO_IN;
    // SMB_CLK_EC
    GPCRC1 = GPIO_ALT;
    // SMB_DATA_EC
    GPCRC2 = GPIO_ALT;
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
    GPCRD0 = GPIO_OUT | GPIO_UP;
    // CCD_EN
    GPCRD1 = GPIO_OUT | GPIO_UP;
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
    // NC
    GPCRD7 = GPIO_IN;
    // SMC_BAT
    GPCRE0 = GPIO_ALT;
    // AC_PRESENT
    GPCRE1 = GPIO_OUT | GPIO_UP;
    // LEDKB_DET#
    GPCRE2 = GPIO_IN;
    // USB_PWR_EN#
    GPCRE3 = GPIO_OUT | GPIO_UP;
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
    // EC_BT_EN
    GPCRF3 = GPIO_OUT | GPIO_UP;
    // TP_CLK
    GPCRF4 = GPIO_ALT;
    // TP_DATA
    GPCRF5 = GPIO_ALT;
    // H_PECI
    GPCRF6 = GPIO_ALT;
    // CPU_C10_GATE#
    GPCRF7 = GPIO_IN | GPIO_DOWN;
    // VCCIN_AUX_PG
    GPCRG0 = GPIO_IN;
    // WLAN_EN
    GPCRG1 = GPIO_OUT | GPIO_UP;
    // Pull up to VDD3?
    GPCRG2 = GPIO_OUT;
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
    // EC_CLKRUN#
    GPCRH0 = GPIO_IN;
    // SUSC#_PCH
    GPCRH1 = GPIO_IN;
    // BKL_EN
    GPCRH2 = GPIO_OUT | GPIO_UP;
    // 3G_EN
    GPCRH3 = GPIO_OUT | GPIO_UP;
    // VR_ON
    GPCRH4 = GPIO_IN;
    // SINK_CTRL_EC
    GPCRH5 = GPIO_IN;
    // SUSB#_PCH
    GPCRH6 = GPIO_IN;
    // ACE_I2C_IRQ2Z
    GPCRH7 = GPIO_OUT;
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
    // NC
    GPCRI5 = GPIO_IN;
    // NC
    GPCRI6 = GPIO_IN;
    // MODEL_ID
    GPCRI7 = GPIO_IN;
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
    // EC_EN
    GPCRJ6 = GPIO_OUT | GPIO_UP;
    // NC
    GPCRJ7 = GPIO_IN;
    // ESPI_IO_0
    GPCRM0 = GPIO_ALT;
    // ESPI_IO_1
    GPCRM1 = GPIO_ALT;
    // ESPI_IO_2
    GPCRM2 = GPIO_ALT;
    // ESPI_IO_3
    GPCRM3 = GPIO_ALT;
    // ESPI_CLK
    GPCRM4 = GPIO_ALT;
    // ESPI_CS_N
    GPCRM5 = GPIO_ALT;
    // SERIRQ
    GPCRM6 = GPIO_ALT;
}

#if GPIO_DEBUG
void gpio_debug_bank(
    char * bank,
    uint8_t data,
    uint8_t mirror,
    uint8_t pot,
    volatile uint8_t * control
) {
    for(char i = 0; i < 8; i++) {
        DEBUG(
            "%s%d: data %d mirror %d pot %d control %02X\n",
            bank,
            i,
            (data >> i) & 1,
            (mirror >> i) & 1,
            (pot >> i) & 1,
            *(control + i)
        );
    }
}

void gpio_debug(void) {
    #define bank(BANK) gpio_debug_bank(#BANK, GPDR ## BANK, GPDMR ## BANK, GPOT ## BANK, &GPCR ## BANK ## 0)
    bank(A);
    bank(B);
    bank(C);
    bank(D);
    bank(E);
    bank(F);
    bank(G);
    bank(H);
    bank(I);
    bank(J);
    #define GPOTM 0
    bank(M);
    #undef GPOTM
    #undef bank
}
#endif
