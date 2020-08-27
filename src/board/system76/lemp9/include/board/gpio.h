#ifndef _BOARD_GPIO_H
#define _BOARD_GPIO_H

#include <ec/gpio.h>

#define GPIO_ALT 0x00
#define GPIO_IN 0x80
#define GPIO_OUT 0x40
#define GPIO_UP 0x04
#define GPIO_DOWN 0x02

void gpio_init(void);
void gpio_debug(void);

extern struct Gpio __code ACIN_N;
extern struct Gpio __code AC_PRESENT;
extern struct Gpio __code ALL_SYS_PWRGD;
extern struct Gpio __code BKL_EN;
extern struct Gpio __code BT_EN;
extern struct Gpio __code BUF_PLT_RST_N;
extern struct Gpio __code CCD_EN;
extern struct Gpio __code DD_ON;
extern struct Gpio __code EC_EN;
extern struct Gpio __code EC_RSMRST_N;
extern struct Gpio __code EC_SMD_EN_N;
#define HAVE_LAN_WAKEUP_N 0
extern struct Gpio __code LED_ACIN;
#define HAVE_LED_AIRPLANE_N 0
#define HAVE_LED_BAT_CHG 0
#define HAVE_LED_BAT_FULL 0
extern struct Gpio __code LED_PWR;
extern struct Gpio __code LID_SW_N;
extern struct Gpio __code PCH_DPWROK_EC;
extern struct Gpio __code PCH_PWROK_EC;
extern struct Gpio __code PM_CLKRUN_N;
extern struct Gpio __code PM_PWROK;
extern struct Gpio __code PWR_BTN_N;
extern struct Gpio __code PWR_SW_N;
extern struct Gpio __code SB_KBCRST_N;
extern struct Gpio __code SCI_N;
extern struct Gpio __code SLP_SUS_N;
extern struct Gpio __code SMI_N;
extern struct Gpio __code SUSB_N_PCH;
extern struct Gpio __code SUSC_N_PCH;
extern struct Gpio __code SUSWARN_N;
extern struct Gpio __code SUS_PWR_ACK;
extern struct Gpio __code SWI_N;
extern struct Gpio __code USB_PWR_EN_N;
extern struct Gpio __code VA_EC_EN;
extern struct Gpio __code VR_ON;
extern struct Gpio __code WLAN_EN;
extern struct Gpio __code WLAN_PWR_EN;
extern struct Gpio __code XLP_OUT;

#endif // _BOARD_GPIO_H
