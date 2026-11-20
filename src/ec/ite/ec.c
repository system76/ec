// SPDX-License-Identifier: GPL-3.0-only

#include <ec/ec.h>
#include <arch/arch.h>
#include <ec/ecpm.h>
#include <ec/gctrl.h>
#include <common/debug.h>
#include <common/macro.h>

static enum EcResetSource ec_lsr = RESET_SOURCE_NORMAL;

enum EcResetSource ec_reset_source(void) {
    return ec_lsr;
}

static void ec_check_reset_source(void) {
    // LSR field in RSTS only reports WDT reset.
    uint8_t rsts = RSTS & 0x03;
    // SPCTRL4 reports more reset sources.
    uint8_t lsr = SPCTRL4;

    // Write-clear reset sources.
    // NOTE: Must only be done once after reset.
    RSTS |= 0x03;
    SPCTRL4 |= 0x0F;

    // XXX: Using a switch statement to check RSTS on SDCC 4.4.0 triggers:
    //      error 110: conditional flow changed by optimizer

    if (rsts == 0 || rsts == 1) {
        // VSTBY or WRST#
        ec_lsr = RESET_SOURCE_NORMAL;
    } else if (rsts == 2 || rsts == 3) {
        // Internal/External WDT
        ec_lsr = RESET_SOURCE_WDT;
    }

    if (lsr & BIT(1)) {
        // PWRSW WDT
        ec_lsr = RESET_SOURCE_PWRSW_TIMEOUT;
    }
}

void ec_read_post_codes(void) {
#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
    while (P80H81HS & 1) {
        uint8_t p80h = P80HD;
        uint8_t p81h = P81HD;
        P80H81HS |= 1;

        DEBUG("POST %02X%02X\n", p81h, p80h);
    }
#endif
}

static void gctrl_init(void) {
    // Set I2EC as R/W
    SPCTRL1 |= 0x03;
    // Set PNPCFG base address
    BADRSEL = 0;
}

static void ecpm_init(void) {
    // Clock gate EGPC, CIR, and SWUC
    CGCTRL2 |= BIT(6) | BIT(5) | BIT(4);
    // Clock gate UART, SSPI, and DBGR
    CGCTRL3 |= BIT(2) | BIT(1) | BIT(0);
    // Clock gate CEC
    CGCTRL4 |= BIT(0);
}

void ec_init(void) {
    arch_init();

    ec_check_reset_source();

#if CONFIG_EC_ITE_IT8587E
    RSTS = (0b10U << 6) | BIT(2);
#else
    RSTS = (0b01U << 6) | BIT(2);

    // Enable POST codes
    SPCTRL1 |= BIT(7) | BIT(6) | BIT(3);
#endif

    gctrl_init();
    ecpm_init();
}
