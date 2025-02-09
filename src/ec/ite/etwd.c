// SPDX-License-Identifier: GPL-3.0-only

// External Timer and External Watchdog (ETWD)

#include <ec/etwd.h>
#include <common/macro.h>

enum EwtCfg {
    // Lock EWTCFG register
    LETWCFG = BIT(0),
    // Lock ET1PS register
    LETPS1 = BIT(1),
    // Lock ET1CNTLx registers
    LET1CNTL = BIT(2),
    // Lock EWDCNTLx registers
    LEWDCNTL = BIT(3),
    // External WDT clock source
    EWDSRC = BIT(4),
    // Enable key match function to touch the WDT
    EWDKEYEN = BIT(5),
    // Lock ET1 and EWDT registers
    LOCK_ALL = LETWCFG | LETPS1 | LET1CNTL | LEWDCNTL,
};

enum EtwdPrescaler {
    ETWD_PRESCALER_32768_HZ = 0,
    ETWD_PRESCALER_1024_HZ = 1,
    ETWD_PRESCALER_32_HZ = 2,
    ETWD_PRESCALER_EC_CLK = 3,      // Not available for ET1PS
};

void wdt_init(void) {
    ET1PSR = ETWD_PRESCALER_1024_HZ;
    ETWCFG = EWDKEYEN | EWDSRC;

    // Start ET1 so EWDT can be started
    ET1CNTLLR = 0xFF;

    // Start EWDT with timeout of 8s
    // TODO: Determine time based on system performance or requirement
    EWDCNTLHR = 0x20;
    EWDCNTLLR = 0;

    ETWCFG |= LOCK_ALL;
}
