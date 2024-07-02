// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_GPIO_H
#define _BOARD_GPIO_H

#include <ec/gpio.h>

void gpio_init(void);

extern struct Gpio __code ACIN_N;
extern struct Gpio __code AC_PRESENT;
extern struct Gpio __code ALL_SYS_PWRGD;
extern struct Gpio __code BKL_EN;
#define HAVE_BT_EN 0
extern struct Gpio __code BUF_PLT_RST_N;
extern struct Gpio __code CCD_EN;
extern struct Gpio __code CPU_C10_GATE_N;
extern struct Gpio __code DD_ON;
extern struct Gpio __code DGPU_PWR_EN;
extern struct Gpio __code EC_EN;
extern struct Gpio __code EC_RSMRST_N;
extern struct Gpio __code GC6_FB_EN;
extern struct Gpio __code JACK_IN_N;
extern struct Gpio __code LAN_WAKEUP_N;
extern struct Gpio __code LED_ACIN;
#define HAVE_LED_AIRPLANE_N 0
extern struct Gpio __code LED_BAT_CHG;
extern struct Gpio __code LED_BAT_FULL;
extern struct Gpio __code LED_PWR;
extern struct Gpio __code LID_SW_N;
extern struct Gpio __code ME_WE;
extern struct Gpio __code PCH_DPWROK_EC;
extern struct Gpio __code PCH_PWROK_EC;
#define HAVE_PM_PWROK 0
extern struct Gpio __code PWR_BTN_N;
extern struct Gpio __code PWR_SW_N;
extern struct Gpio __code RGBKB_DET_N;
extern struct Gpio __code SINK_CTRL;
extern struct Gpio __code SLP_SUS_N;
#define HAVE_SUS_PWR_ACK 0
extern struct Gpio __code VA_EC_EN;
extern struct Gpio __code WLAN_EN;
extern struct Gpio __code WLAN_PWR_EN;
extern struct Gpio __code XLP_OUT;

#endif // _BOARD_GPIO_H
