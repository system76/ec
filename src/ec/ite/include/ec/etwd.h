// SPDX-License-Identifier: GPL-3.0-only

// External Timer and External Watchdog (ETWD)

#ifndef _EC_ETWD_H
#define _EC_ETWD_H

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

// When the key match function of EWD is enabled (EWTCFG[5]), writing this
// value to EWDKEY will restart the WDT.
#define WDT_KEY 0x5C

void wdt_init(void);

// Restart WDT
// NOTE: Must be inlined for compiling in Scratch ROM
static inline void wdt_kick(void) {
    EWDKEYR = WDT_KEY;
}

// Trigger EC reset by WDT key mismatch
// NOTE: Must be inlined for compiling in Scratch ROM
static inline void wdt_trigger(void) {
    EWDKEYR = 0;
}

#endif // _EC_ETWD_H
