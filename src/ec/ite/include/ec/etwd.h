// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_ECWD_H
#define _EC_ECWD_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x1F01) ETWCFG;
volatile uint8_t __xdata __at(0x1F02) ET1PSR;
#if CONFIG_EC_ITE_IT8587E
volatile uint8_t __xdata __at(0x1F03) ET1CNTLHR;
#endif
volatile uint8_t __xdata __at(0x1F04) ET1CNTLLR;
volatile uint8_t __xdata __at(0x1F05) ETWCTRL;
volatile uint8_t __xdata __at(0x1F06) EWDCNTLLR;
volatile uint8_t __xdata __at(0x1F07) EWDKEYR;
volatile uint8_t __xdata __at(0x1F09) EWDCNTLHR;
volatile uint8_t __xdata __at(0x1F0A) ET2PSR;
volatile uint8_t __xdata __at(0x1F0B) ET2CNTLHR;
volatile uint8_t __xdata __at(0x1F0C) ET2CNTLLR;
volatile uint8_t __xdata __at(0x1F0E) ET2CNTLH2R;
#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
volatile uint8_t __xdata __at(0x1F10) ET3PSR;
volatile uint8_t __xdata __at(0x1F11) ET3CNTLHR;
volatile uint8_t __xdata __at(0x1F12) ET3CNTLLR;
volatile uint8_t __xdata __at(0x1F13) ET3CNTLH2R;
volatile uint8_t __xdata __at(0x1F16) ET4CNTLLR;
#endif

#endif // _EC_ECWD_H
