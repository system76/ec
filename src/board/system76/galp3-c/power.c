#include <arch/delay.h>
#include <ec/gpio.h>

// Platform does not currently support Deep Sx
#define DEEP_SX 0

static struct Gpio __code PCH_DPWROK_EC =   GPIO(A, 3);
static struct Gpio __code PCH_PWROK_EC =    GPIO(A, 4);
static struct Gpio __code LED_PWR =         GPIO(A, 7);
static struct Gpio __code ALL_SYS_PWRGD =   GPIO(C, 0);
static struct Gpio __code PM_PWROK =        GPIO(C, 6);
static struct Gpio __code PWR_SW_N =        GPIO(D, 0);
static struct Gpio __code BUF_PLT_RST_N =   GPIO(D, 2);
static struct Gpio __code PWR_BTN_N =       GPIO(D, 5);
static struct Gpio __code SUSWARN_N =       GPIO(D, 7);
static struct Gpio __code EC_EN =           GPIO(E, 1);
static struct Gpio __code VA_EC_EN =        GPIO(E, 3);
static struct Gpio __code DD_ON =           GPIO(E, 4);
static struct Gpio __code EC_RSMRST_N =     GPIO(E, 5);
static struct Gpio __code AC_PRESENT =      GPIO(E, 7);
static struct Gpio __code SUSC_N_PCH =      GPIO(H, 1);
static struct Gpio __code VR_ON =           GPIO(H, 4);
static struct Gpio __code SUSB_N_PCH =      GPIO(H, 6);
static struct Gpio __code SLP_SUS_N =       GPIO(I, 2);
static struct Gpio __code SUS_PWR_ACK =     GPIO(J, 0);

// VccRTC stable (55%) to RTCRST# high
#define tPCH01 delay_ms(9)
// VccDSW stable (95%) to RSMRST# high
#define tPCH02 delay_ms(10)
// VccPrimary stable (95%) to RSMRST# high
#define tPCH03 delay_ms(10)
// VccRTC stable (90%) to start of VccDSW voltage ramp
#define tPCH04 delay_ms(9)
// RTCRST# high to DSW_PWROK
#define tPCH05 delay_us(1)
// VccDSW 3.3 stable to VccPrimary 1.05V
#define tPCH06 delay_us(200)
// DSW_PWROK high to RSMRST# high
#define tPCH07 delay_ms(0)
// SLP_S3# de-assertion to PCH_PWROK assertion
#define tPCH08 delay_ms(1)
// SLP_A# high when ASW rails are stable (95%)
#define tPCH09 delay_ms(2, 4, 8, 16) //TODO
// PCH_PWROK low to VCCIO dropping 5%
#define tPCH10 delay_ns(400)
// SLP_SUS# asserting to VccPRIM dropping 5%
#define tPCH11 delay_ns(100)
// RSMRST# asserting to VccPRIM dropping 5%
#define tPCH12 delay_ns(400)
// DSW_PWROK assertion to SLP_SUS# de-assertion
#define tPCH32 delay_ms(95)

// Enable deep sleep well power
int power_on_dsw() {
#if DEEP_SX
    // See Figure 12-18 in Whiskey Lake Platform Design Guide
    // | VCCRTC | RTCRST# | VCCDSW_3P3 | DSW_PWROK |
    // | tPCH01---------- |            |           |
    // | tPCH04----------------------- |           |
    // |        | tPCH05-------------------------- |
    // |        |         | tPCH02---------------- |

    // tPCH01 and tPCH02 combined make the longest delay
    tPCH01;
    tPCH02;

    // Deep sleep well is a-ok
    gpio_set(PCH_DPWROK_EC, true);
    // Wait for deep sleep well to propogate
    tPCH32;
#else // DEEP_SX
    // See Figure 12-19 in Whiskey Lake Platform Design Guide
    // | VCCRTC | RTCRST# | VccPRIM |
    // | tPCH01---------- |         |
    // | tPCH04-------------------- |

    // tPCH04 is the ideal delay
    tPCH04;
#endif // DEEP_SX

    // We are ready to move on to the next stage
    return 0;
}

// Enable S5 power
void power_on_s5() {
#if DEEP_SX
    // See Figure 12-18 in Whiskey Lake Platform Design Guide

    // RTC stage
    {
        // | VCCRTC | RTCRST# | VCCDSW_3P3 | DSW_PWROK |
        // | tPCH01---------- |            |           |
        // | tPCH04----------------------- |           |
        // |        | tPCH05-------------------------- |
        // |        |         | tPCH02---------------- |
        // tPCH01 and tPCH02 combined make the longest delay
        tPCH01;
        tPCH02;
    }

    // Deep sleep well is a-ok
    gpio_set(PCH_DPWROK_EC, true);
    tPCH32;


    // TODO
#else // DEEP_SX
    // See Figure 12-19 in Whiskey Lake Platform Design Guide

    // TODO
#endif // DEEP_SX
}
