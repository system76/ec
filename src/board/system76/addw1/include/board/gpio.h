// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_GPIO_H
#define _BOARD_GPIO_H

#include <ec/gpio.h>

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
extern struct Gpio __code DGPU_PWR_EN;
extern struct Gpio __code EC_EN;
extern struct Gpio __code EC_RSMRST_N;
extern struct Gpio __code GC6_FB_EN;
extern struct Gpio __code LAN_WAKEUP_N;
extern struct Gpio __code LED_ACIN;
extern struct Gpio __code LED_AIRPLANE_N;
extern struct Gpio __code LED_CAP_N;
extern struct Gpio __code LED_BAT_CHG;
extern struct Gpio __code LED_BAT_FULL;
extern struct Gpio __code LED_NUM_N;
extern struct Gpio __code LED_PWR;
extern struct Gpio __code LED_SCROLL_N;
extern struct Gpio __code LID_SW_N;
#define HAVE_PCH_DPWROK_EC 0
#define HAVE_PCH_PWROK_EC 0
extern struct Gpio __code PM_CLKRUN_N;
extern struct Gpio __code PM_PWROK;
extern struct Gpio __code PWR_BTN_N;
extern struct Gpio __code PWR_SW_N;
extern struct Gpio __code SB_KBCRST_N;
extern struct Gpio __code SCI_N;
#define HAVE_SLP_SUS_N 0
extern struct Gpio __code SMI_N;
extern struct Gpio __code SUSB_N_PCH;
extern struct Gpio __code SUSC_N_PCH;
#define HAVE_SUSWARN_N 0
#define HAVE_SUS_PWR_ACK 0
extern struct Gpio __code SWI_N;
extern struct Gpio __code USB_PWR_EN_N;
extern struct Gpio __code VA_EC_EN;
#define HAVE_VR_ON 0
extern struct Gpio __code WLAN_EN;
extern struct Gpio __code WLAN_PWR_EN;
#define HAVE_XLP_OUT 0

#endif // _BOARD_GPIO_H
