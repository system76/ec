#include <board/gpio.h>
#include <common/debug.h>

struct Gpio __code ACIN_N =         GPIO(B, 0);
struct Gpio __code AC_PRESENT =     GPIO(E, 1);
struct Gpio __code ALL_SYS_PWRGD =  GPIO(C, 0);
struct Gpio __code BKL_EN =         GPIO(H, 2);
struct Gpio __code BUF_PLT_RST_N =  GPIO(D, 2);
struct Gpio __code CCD_EN =         GPIO(D, 1);
struct Gpio __code DD_ON =          GPIO(E, 4);
struct Gpio __code EC_EN =          GPIO(B, 6); // renamed to SUSBC_EN
struct Gpio __code EC_RSMRST_N =    GPIO(E, 5);
struct Gpio __code LED_ACIN =       GPIO(C, 7);
struct Gpio __code LED_AIRPLANE_N = GPIO(H, 7);
struct Gpio __code LED_CAP_N =      GPIO(J, 2);
struct Gpio __code LED_BAT_CHG =    GPIO(H, 5);
struct Gpio __code LED_BAT_FULL =   GPIO(J, 0);
struct Gpio __code LED_NUM_N =      GPIO(G, 0);
struct Gpio __code LED_PWR =        GPIO(D, 0);
struct Gpio __code LED_SCROLL_N =   GPIO(J, 3);
struct Gpio __code LID_SW_N =       GPIO(B, 1);
struct Gpio __code PM_CLKRUN_N =    GPIO(H, 0); // renamed to ECCLKRUN#
struct Gpio __code PM_PWROK =       GPIO(C, 6);
struct Gpio __code PWR_BTN_N =      GPIO(D, 5);
struct Gpio __code PWR_SW_N =       GPIO(B, 3);
struct Gpio __code SCI_N =          GPIO(D, 3);
struct Gpio __code SMI_N =          GPIO(D, 4);
struct Gpio __code SUSB_N_PCH =     GPIO(H, 6);
struct Gpio __code SUSC_N_PCH =     GPIO(H, 1);
struct Gpio __code SWI_N =          GPIO(B, 5);
struct Gpio __code VA_EC_EN =       GPIO(J, 4);
struct Gpio __code WLAN_EN =        GPIO(G, 1);
struct Gpio __code WLAN_PWR_EN =    GPIO(J, 7);
struct Gpio __code XLP_OUT =        GPIO(B, 4);

void gpio_init() {
    // Enable LPC reset on GPD2
    GCR = 0x04;
    // Enable SMBus channel 4
    GCR15 = (1 << 4);
    // Set GPF2 and GPF3 to 3.3V
    GCR20 = 0;

    // Set GPIO data
    GPDRA = 0x00;
    GPDRB = 0x18;
    GPDRC = 0x00;
    GPDRD = 0x38;
    GPDRE = 0x40;
    GPDRF = 0x40;
    GPDRG = 0x41;
    GPDRH = 0x80;
    GPDRI = 0x00;
    GPDRJ = 0x0C;

    // Set GPIO control
    // EC_PWM_LEDKB_P
    GPCRA0 = GPIO_ALT;
    // KBC_BEEP
    GPCRA1 = GPIO_ALT;
    // CPU_FAN
    GPCRA2 = GPIO_ALT;
    // BRIGHTNESS
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
    GPCRB2 = GPIO_IN | GPIO_UP;
    // PWR_SW#
    GPCRB3 = GPIO_IN;
    // XLP_OUT
    GPCRB4 = GPIO_OUT;
    // SWI#
    GPCRB5 = GPIO_OUT | GPIO_UP;
    // SUSBC_EN
    GPCRB6 = GPIO_OUT | GPIO_UP;
    // 
    GPCRB7 = GPIO_IN;
    // ALL_SYS_PWRGD
    GPCRC0 = GPIO_IN;
    // SMC_VGA_THERM
    GPCRC1 = GPIO_ALT;
    // SMD_VGA_THERM
    GPCRC2 = GPIO_ALT;
    // KB-SO16
    GPCRC3 = 0x04;
    // CNVI_DET#_EC
    GPCRC4 = GPIO_IN | GPIO_UP;
    // KB-SO17
    GPCRC5 = 0x04;
    // PM_PWEROK
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
    GPCRD6 = GPIO_IN;
    // VGA_FANSEN
    GPCRD7 = GPIO_IN;
    // SMC_BAT_EC
    GPCRE0 = GPIO_ALT;
    // AC_PRESENT
    GPCRE1 = GPIO_OUT | GPIO_DOWN;
    // PERKB-DET#
    GPCRE2 = GPIO_IN | GPIO_UP;
    // BL_PWM_EN_EC
    GPCRE3 = GPIO_OUT | GPIO_UP;
    // DD_ON
    GPCRE4 = GPIO_OUT | GPIO_DOWN;
    // EC_RSMRST#
    GPCRE5 = GPIO_OUT;
    // PLVDD_RST_EC
    GPCRE6 = GPIO_OUT | GPIO_UP;
    // SMD_BAT_EC
    GPCRE7 = GPIO_ALT;
    // 80CLK
    GPCRF0 = GPIO_IN;
    // USB_CHARGE_EN
    GPCRF1 = GPIO_OUT | GPIO_UP;
    // 3IN1
    GPCRF2 = GPIO_IN | GPIO_UP;
    // MUX_CTRL_BIOS
    GPCRF3 = GPIO_OUT | GPIO_UP;
    // TP_CLK
    GPCRF4 = GPIO_IN;
    // TP_DATA
    GPCRF5 = GPIO_IN;
    // EC_PECI
    GPCRF6 = GPIO_ALT;
    // SLP_S0#
    GPCRF7 = GPIO_IN | GPIO_UP;
    // LED_NUM#
    GPCRG0 = GPIO_OUT | GPIO_UP;
    // WLAN_EN
    GPCRG1 = GPIO_OUT | GPIO_UP;
    // AUTO_LOAD
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
    // ECCLKRUN#
    GPCRH0 = GPIO_ALT;
    // SUSC#_PCH
    GPCRH1 = GPIO_IN;
    // BKL_EN
    GPCRH2 = GPIO_OUT | GPIO_UP;
    // GC6_FB_EN_PCH
    GPCRH3 = GPIO_IN;
    // DGPU_PWR_EN
    GPCRH4 = GPIO_IN;
    // LED_BAT_CHG
    GPCRH5 = GPIO_OUT | GPIO_UP;
    // SUSB#_PCH
    GPCRH6 = GPIO_IN;
    // AIRPLAN_LED#
    GPCRH7 = GPIO_OUT | GPIO_UP;
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
    // RGBKB-DET#
    GPCRI5 = GPIO_IN | GPIO_UP;
    // OVERT#_EC
    GPCRI6 = GPIO_IN;
    // MODEL_ID
    GPCRI7 = GPIO_IN;
    // LED_BAT_FULL
    GPCRJ0 = GPIO_OUT | GPIO_UP;
    // KBC_MUTE#
    GPCRJ1 = GPIO_IN;
    // LED_CAP#
    GPCRJ2 = GPIO_OUT | GPIO_UP;
    // LED_SCROLL#
    GPCRJ3 = GPIO_OUT | GPIO_UP;
    // VA_EC_EN
    GPCRJ4 = GPIO_OUT;
    // VBATT_BOOST#
    GPCRJ5 = GPIO_OUT;
    // POWER_IC_EN
    GPCRJ6 = GPIO_OUT | GPIO_UP;
    // WLAN_PWR_EN
    GPCRJ7 = GPIO_OUT | GPIO_UP;
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
