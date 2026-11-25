// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 System76, Inc.

// Legacy PECI implementation
// Uses PECI module with dedicated wire.
//
// Requires:
// - `CONFIG_PLATFORM_INTEL`: Intel proprietary interface
// - Dedicated wire connected to the PCH and EC (H_PECI)
// - GPCRF6=ALT

#include <drivers/peci/peci.h>
#include <ec/peci.h>
#include <arch/time.h>
#include <common/debug.h>
#include <common/errno.h>
#include <common/macro.h>
#include <ec/gctrl.h>
#include <ec/gpio.h>

struct PeciReq peci_req = { { 0 } };
struct PeciResp peci_resp = { { 0 } };

// RSTC4
#define RPECI           BIT(4)  // Reset PECI module

// HOSTAR
#define HOBY            BIT(0)  // Host Busy
#define FINISH          BIT(1)  // Finish
#define RD_FCS_ERR      BIT(2)  // Read_FCS Error
#define WR_FCS_ERR      BIT(3)  // Write_FCS Error
#define EXTERR          BIT(5)  // PECI Slave Message Phase T-bit Extend over Error
#define BUSERR          BIT(6)  // PECI Bus Abnormal/Contention Error
#define RCV_ERRCODE     BIT(7)  // PECI GetTemp() Command Receive Error Code
#define HW_ERR          (RCV_ERRCODE | BUSERR | EXTERR | WR_FCS_ERR | RD_FCS_ERR)

// HOCTLR
#define START           BIT(0)  // Start
#define AWFCS_EN        BIT(1)  // Assured Write FCS Enable
#define CONCTRL         BIT(2)  // PECI Contention Control
#define PECIHEN         BIT(3)  // PECI Host Enable
#define FCSERR_ABT      BIT(4)  // PECI Host Auto-abort at FCS_Error
#define FIFOCLR         BIT(5)  // Data FIFO Pointer Clear
#define AWFCS_FRC_CTRL  BIT(6)  // AW_FCS Force Control

// HOCTL2R[2:0]: Host Optimal Transfer Rate Setting (HOPTTRS)
#define HOPTTRS_1P8MHZ  0b000
#define HOPTTRS_1MHZ    0b001
#define HOPTTRS_1P5MHZ  0b100

// PADCTLR[1:0]: Host Vtt Setting (HOVTTS)
#define HOVTTS_1P10V    0b00
#define HOVTTS_1P05V    0b01
#define HOVTTS_1P00V    0b10
#define HOVTTS_0P90V    0b11

static inline void peci_clear_status(void) {
    // HOBY bit is RO, all others are R/WC.
    HOSTAR = HW_ERR | FINISH;
}

static void peci_reset(void) {
    ERROR("PECI module reset\n");
    RSTC4 |= RPECI;

    HOCTL2R = HOPTTRS_1MHZ;
    PADCTLR = HOVTTS_1P00V;
}

static int8_t peci_wait_busy(void) {
    systick_t start = time_get();

    while (HOSTAR & HOBY) {
        if ((time_get() - start) > PECI_TIMEOUT_MS) {
            return -EBUSY;
        }
    }

    return 0;
}

static int8_t peci_wait_finish(void) {
    systick_t start = time_get();

    while (!(HOSTAR & FINISH)) {
        if ((time_get() - start) > PECI_TIMEOUT_MS) {
            return -ETIMEDOUT;
        }
    }

    return 0;
}

// TODO: Allow retry for supported functions; Retry bit should be set.
int8_t peci_transaction(void) {
    int8_t err;

    err = peci_wait_busy();
    if (err) {
        WARN("PECI host busy\n");
        return err;
    }

    HOCTLR |= FIFOCLR | FCSERR_ABT | PECIHEN | CONCTRL | AWFCS_EN;

    HOTRADDR = peci_req.addr;
    HOWRLR = peci_req.wr_len;
    HORDLR = peci_req.rd_len;
    HOCMDR = peci_req.cmd;

    // Write in WR_DAT FIFO bytes
    if (peci_req.wr_len > 1) {
        for (uint8_t i = 0; i < peci_req.wr_len - 1; i++) {
            HOWRDR = peci_req.data[i];
        }
    }

    HOCTLR |= START;

    // XXX: Reset module to default state on hang or HW err?
    err = peci_wait_finish();
    if (err) {
        peci_clear_status();
        return err;
    }

    uint8_t status = HOSTAR;
    if (status & HW_ERR) {
        ERROR("PECI hardware error: %02X\n", status);
        peci_clear_status();
        return -EIO;
    }

    if (peci_req.cmd == PECI_CMD_PING) {
        // Ping() returns 1 byte non-zero Write FCS if device is available.
        peci_resp.data[0] = RWFCSV;
    } else {
        // Read out RD_DAT FIFO bytes
        for (uint8_t i = 0; i < peci_req.rd_len; i++) {
            peci_resp.data[i] = HORDDR;
        }
    }

    peci_clear_status();
    return 0;
}

void peci_init(void) {
    // NOTE: GPF6 set to ALT mode in board GPIO
    GCR2 |= PECIE;

    HOCTL2R = HOPTTRS_1MHZ;
    PADCTLR = HOVTTS_1P00V;
}
