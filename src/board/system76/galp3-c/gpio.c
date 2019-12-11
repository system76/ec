#include <board/gpio.h>
#include <common/debug.h>

#define GPIO_ALT 0x00
#define GPIO_IN 0x80
#define GPIO_OUT 0x40
#define GPIO_UP 0x04
#define GPIO_DOWN 0x02

void gpio_init() {
    // Enable LPC reset on GPD2
    GCR = 0x04;

    // Set GPIO data
    GPDRA = 0;
    GPDRB = (1 << 0);
    GPDRC = 0;
    GPDRD = (1 << 5) | (1 << 4) | (1 << 3);
    GPDRE = 0;
    GPDRF = 0xC0; // (1 << 7) | (1 << 6)
    GPDRG = 0;
    GPDRH = 0;
    GPDRI = 0;
    GPDRJ = 0;

    // Set GPIO control
    // EC_SSD_LED#
    GPCRA0 = GPIO_IN;
    // KBC_BEEP
    GPCRA1 = GPIO_ALT;
    // CPU_FAN
    GPCRA2 = GPIO_ALT;
    // PCH_DPWROK_EC
    GPCRA3 = GPIO_IN;
    // PCH_PWROK_EC
    GPCRA4 = GPIO_OUT;
    // LED_BAT_CHG
    GPCRA5 = GPIO_OUT | GPIO_UP;
    // LED_BAT_FULL
    GPCRA6 = GPIO_OUT | GPIO_UP;
    // LED_PWR
    GPCRA7 = GPIO_OUT | GPIO_UP;
    // NC
    GPCRB0 = GPIO_OUT | GPIO_UP;
    // H_PROCHOT_EC
    GPCRB1 = GPIO_OUT | GPIO_UP;
    // LAN_WAKEUP#
    GPCRB2 = GPIO_IN | GPIO_UP;
    // SMC_BAT
    GPCRB3 = GPIO_ALT;
    // SMD_BAT
    GPCRB4 = GPIO_ALT;
    // GA20
    GPCRB5 = GPIO_OUT | GPIO_UP;
    // AC_IN#
    GPCRB6 = GPIO_IN | GPIO_UP;
    // FP_RST#
    GPCRB7 = GPIO_IN;
    // ALL_SYS_PWRGD
    GPCRC0 = GPIO_IN;
    // SMC_VGA_THERM
    GPCRC1 = GPIO_IN | GPIO_UP;
    // SMD_VGA_THERM
    GPCRC2 = GPIO_IN | GPIO_UP;
    // NC
    GPCRC3 = GPIO_IN | GPIO_UP;
    // CNVI_DET#
    GPCRC4 = GPIO_OUT | GPIO_UP;
    // NC
    GPCRC5 = GPIO_OUT | GPIO_UP;
    // PM_PWROK
    GPCRC6 = GPIO_OUT;
    // LED_ACIN
    GPCRC7 = GPIO_OUT | GPIO_UP;
    // PWR_SW#
    GPCRD0 = GPIO_IN | GPIO_UP;
    // LID_SW#
    GPCRD1 = GPIO_IN | GPIO_UP;
    // BUF_PLT_RST#
    GPCRD2 = GPIO_ALT;
    // SMI#
    GPCRD3 = GPIO_IN;
    // SCI#
    GPCRD4 = GPIO_IN;
    // PWR_BTN#
    GPCRD5 = GPIO_OUT | GPIO_UP;
    // CPU_FANSEN
    GPCRD6 = GPIO_IN;
    // SUSWARN#
    GPCRD7 = GPIO_IN;
    // SWI#
    GPCRE0 = GPIO_OUT | GPIO_UP;
    // EC_EN
    GPCRE1 = GPIO_OUT | GPIO_UP;
    // PCH_SLP_WLAN#_R
    GPCRE2 = GPIO_IN;
    // VA_EC_EN
    GPCRE3 = GPIO_OUT;
    // DD_ON
    GPCRE4 = GPIO_OUT | GPIO_DOWN;
    // EC_RSMRST#
    GPCRE5 = GPIO_OUT;
    // SB_KBCRST#
    GPCRE6 = GPIO_OUT | GPIO_UP;
    // AC_PRESENT / PM_PWROK
    GPCRE7 = GPIO_OUT | GPIO_UP;
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
    // H_PECI
    GPCRF6 = GPIO_ALT;
    // USB_PWR_EN#
    GPCRF7 = GPIO_OUT;
    // CCD_EN
    GPCRG0 = GPIO_OUT | GPIO_UP;
    // 3G_EN
    GPCRG1 = GPIO_OUT | GPIO_UP;
    // VDD3
    GPCRG2 = GPIO_OUT;
    // HSPI_CE#
    GPCRG3 = GPIO_ALT;
    // HSPI_MSI
    GPCRG4 = GPIO_ALT;
    // HSPI_MSO
    GPCRG5 = GPIO_ALT;
    // AIRPLAN_LED#
    GPCRG6 = GPIO_OUT | GPIO_UP;
    // HCPI_SCLK
    GPCRG7 = GPIO_ALT;
    // EC_CLKRUN#
    GPCRH0 = GPIO_ALT;
    // SUSC#_PCH
    GPCRH1 = GPIO_IN;
    // BKL_EN
    GPCRH2 = GPIO_OUT | GPIO_UP;
    // NC
    GPCRH3 = GPIO_OUT | GPIO_UP;
    // VR_ON
    GPCRH4 = GPIO_IN;
    // WLAN_EN
    GPCRH5 = GPIO_OUT | GPIO_UP;
    // SUSB#_PCH
    GPCRH6 = GPIO_IN;
    // Unknown
    GPCRH7 = GPIO_IN;
    // BAT_DET
    GPCRI0 = GPIO_ALT;
    // BAT_VOLT
    GPCRI1 = GPIO_ALT;
    // SLP_SUS#
    GPCRI2 = GPIO_IN;
    // THERM_VOLT
    GPCRI3 = GPIO_ALT;
    // TOTAL_CUR
    GPCRI4 = GPIO_ALT;
    // AZ_RST#_EC
    GPCRI5 = GPIO_IN;
    // LIGHT_KB_DET#
    GPCRI6 = GPIO_IN;
    // MODEL_ID
    GPCRI7 = GPIO_IN;
    // SUS_PWR_ACK
    GPCRJ0 = GPIO_IN | GPIO_DOWN;
    // KBC_MUTE#
    GPCRJ1 = GPIO_IN;
    // ME_WE
    GPCRJ2 = GPIO_OUT;
    // SOC_TYPE
    GPCRJ3 = GPIO_IN;
    // WLAN_PWR_EN
    GPCRJ4 = GPIO_OUT | GPIO_UP;
    // KBLIGHT_ADJ
    GPCRJ5 = GPIO_OUT;
    // 3G_PWR_EN
    GPCRJ6 = GPIO_OUT | GPIO_UP;
    // NC
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
            "%s%d:\n\tdata %d\n\tmirror %d\n\tpot %d\n\tcontrol %02X\n",
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
    #undef bank
}
#endif
