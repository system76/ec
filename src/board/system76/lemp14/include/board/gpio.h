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
extern struct Gpio __code EC_EN;
extern struct Gpio __code EC_RSMRST_N;
#define HAVE_LAN_WAKEUP_N 0
extern struct Gpio __code LED_ACIN;
#define HAVE_LED_AIRPLANE_N 0
#define HAVE_LED_BAT_CHG 0
#define HAVE_LED_BAT_FULL 0
extern struct Gpio __code LED_PWR;
extern struct Gpio __code LID_SW_N;
extern struct Gpio __code ME_WE;
#define HAVE_PCH_DPWROK_EC 0
extern struct Gpio __code PCH_PWROK_EC;
#define HAVE_PD_EN 1
extern struct Gpio __code PD_EN;
#define HAVE_PM_PWROK 0
extern struct Gpio __code PWR_BTN_N;
extern struct Gpio __code PWR_SW_N;
extern struct Gpio __code SLP_S0_N;
#define HAVE_SLP_SUS_N 0
extern struct Gpio __code SUSB_N_PCH;
extern struct Gpio __code SUSC_N_PCH;
#define HAVE_SUSWARN_N 0
#define HAVE_SUS_PWR_ACK 0
extern struct Gpio __code VA_EC_EN;
#define HAVE_WLAN_EN 0
extern struct Gpio __code WLAN_PWR_EN;
extern struct Gpio __code XLP_OUT;

#endif // _BOARD_GPIO_H
