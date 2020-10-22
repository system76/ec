// SPDX-License-Identifier: GPL-3.0-only

#include <board/gpio.h>
#include <common/debug.h>
#include <common/macro.h>

struct Gpio __code ACIN_N =         GPIO(B, 0);
struct Gpio __code AC_PRESENT =     GPIO(E, 1);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(H, 2);
struct Gpio __code BT_EN =          GPIO(F, 3);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2); // renamed to EC_ERST#
struct Gpio __code CCD_EN =         GPIO(D, 1);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code DGPU_PWR_EN =    GPIO(H, 4);
struct Gpio __code EC_EN =          GPIO(B, 6); // renamed to SUSBC_EN
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code GC6_FB_EN =      GPIO(J, 3);
struct Gpio __code LAN_WAKEUP_N =   GPIO(B, 2);
struct Gpio __code LED_ACIN =       GPIO(C, 7);
struct Gpio __code LED_BAT_CHG =    GPIO(H, 5);
struct Gpio __code LED_BAT_FULL =   GPIO(J, 0);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code PCH_DPWROK_EC =  GPIO(C, 5);
struct Gpio __code PCH_PWROK_EC =   GPIO(A, 6);
struct Gpio __code PM_CLKRUN_N =    GPIO(H, 0);
struct Gpio __code PM_PWROK =       GPIO(C, 6);
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code SB_KBCRST_N =    GPIO(E, 6);
//TODO eSPI: struct Gpio __code SCI_N =          GPIO();
struct Gpio __code SLP_SUS_N =      GPIO(J, 7);
struct Gpio __code SMI_N =          GPIO(D, 4);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
//TODO eSPI: struct Gpio __code SUSWARN_N =      GPIO();
//TODO eSPI: struct Gpio __code SUS_PWR_ACK =    GPIO();
struct Gpio __code SWI_N =          GPIO(B, 5);
struct Gpio __code USB_PWR_EN_N =   GPIO(E, 3);
struct Gpio __code VA_EC_EN =       GPIO(J, 4);
struct Gpio __code WLAN_EN =        GPIO(G, 1);
struct Gpio __code WLAN_PWR_EN =    GPIO(A, 3);
struct Gpio __code XLP_OUT =        GPIO(B, 4);

void gpio_init() {
    // Enable LPC reset on GPD2
    GCR = 0x04;
    // Enable SMBus channel 4
    GCR15 = BIT(4);
    // Set GPD2 to 1.8V
    GCR19 = BIT(0);
    // Set GPF2 and GPF3 to 3.3V
    GCR20 = 0;
    // Set GPH0 to 1.8V
    GCR21 = BIT(2);

    // Set GPIO data
    GPDRA = 0;
    // XLP_OUT, PWR_SW#
    GPDRB = BIT(4) | BIT(3);
    // PCH_DPWROK_EC
    GPDRC = BIT(5);
    // PWR_BTN#, SMI#
    GPDRD = BIT(5) | BIT(4);
    GPDRE = 0;
    // H_PECI
    GPDRF = BIT(6);
    GPDRG = 0;
    GPDRH = 0;
    GPDRI = 0;
    // KBC_MUTE#
    GPDRJ = BIT(1);
    GPDRM = 0;

    // Set GPIO control
    // ME_WE
    GPCRA0 = GPIO_OUT;
    // KBC_BEEP
    GPCRA1 = GPIO_ALT;
    // CPU_FAN
    GPCRA2 = GPIO_ALT;
    // WLAN_PWR_EN
    GPCRA3 = GPIO_OUT | GPIO_UP;
    // VGA_FAN
    GPCRA4 = GPIO_ALT;
    // SLP_S0#
    GPCRA5 = GPIO_IN;
    // PCH_PWROK_EC
    GPCRA6 = GPIO_OUT | GPIO_UP;
    // PCH_SLP_WLAN#_R
    GPCRA7 = GPIO_IN;
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
    // SWI#
    GPCRB5 = GPIO_OUT | GPIO_UP;
    // SUSBC_EC
    GPCRB6 = GPIO_OUT | GPIO_UP;
    // N/A
    GPCRB7 = GPIO_IN;
    // ALL_SYS_PWRGD
    GPCRC0 = GPIO_IN;
    // SMC_VGA_THERM
    GPCRC1 = GPIO_ALT | GPIO_UP;
    // SMD_VGA_THERM
    GPCRC2 = GPIO_ALT | GPIO_UP;
    // SINK_CTRL_EC
    GPCRC3 = GPIO_IN;
    // CNVI_DET#
    GPCRC4 = GPIO_IN | GPIO_UP;
    // PCH_DPWROK_EC
    GPCRC5 = GPIO_OUT | GPIO_UP;
    // PM_PWROK
    GPCRC6 = GPIO_OUT;
    // LED_ACIN
    GPCRC7 = GPIO_OUT | GPIO_UP;
    // LED_PWR
    GPCRD0 = GPIO_OUT | GPIO_UP;
    // CCD_EN
    GPCRD1 = GPIO_OUT | GPIO_UP;
    // EC_ERST#
    GPCRD2 = GPIO_ALT;
    // CPU_C10_GATE#
    GPCRD3 = GPIO_IN | GPIO_DOWN;
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
    // KB-DET
    GPCRE2 = GPIO_IN | GPIO_UP;
    // USB_PWR_EN#
    GPCRE3 = GPIO_OUT | GPIO_UP;
    // DD_ON
    GPCRE4 = GPIO_OUT | GPIO_DOWN;
    // EC_RSMRST#
    GPCRE5 = GPIO_OUT;
    // SB_KBCRST#
    GPCRE6 = GPIO_IN;
    // SMD_BAT
    GPCRE7 = GPIO_ALT | GPIO_UP;
    // 80CLK
    GPCRF0 = GPIO_OUT | GPIO_UP;
    // USB_CHARGE_EN
    GPCRF1 = GPIO_OUT | GPIO_UP;
    // 3IN1
    GPCRF2 = GPIO_OUT | GPIO_UP;
    // BT_EN
    GPCRF3 = 0x06;
    // TP_CLK
    GPCRF4 = GPIO_ALT | GPIO_UP;
    // TP_DATA
    GPCRF5 = GPIO_ALT | GPIO_UP;
    // H_PECI
    GPCRF6 = GPIO_ALT;
    // CC_EN
    GPCRF7 = GPIO_OUT | GPIO_UP;
    // dGPU_GPIO8_OVERT
    GPCRG0 = GPIO_IN;
    // WLAN_EN
    GPCRG1 = GPIO_OUT | GPIO_UP;
    // 10K pull-up
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
    // PM_CLKRUN#
    GPCRH0 = GPIO_IN;
    // SUSC#_PCH
    GPCRH1 = GPIO_IN;
    // BKL_EN
    GPCRH2 = GPIO_OUT | GPIO_UP;
    // PCIE_WAKE#
    GPCRH3 = GPIO_IN;
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
    // ASM1543_CC1
    GPCRI5 = GPIO_ALT;
    // ASM1543_CC2
    GPCRI6 = GPIO_ALT;
    // MODEL_ID
    GPCRI7 = GPIO_IN;
    // LED_BAT_FULL
    GPCRJ0 = GPIO_OUT | GPIO_UP;
    // KBC_MUTE#
    GPCRJ1 = GPIO_OUT;
    // KBLIGHT_ADJ
    GPCRJ2 = GPIO_ALT;
    // GC6_FB_EN
    GPCRJ3 = GPIO_IN;
    // VA_EC_EN
    GPCRJ4 = GPIO_OUT;
    // VBATT_BOOST#
    GPCRJ5 = GPIO_OUT;
    // EC_GPIO
    GPCRJ6 = GPIO_OUT | GPIO_UP;
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
    // ALERT#
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
