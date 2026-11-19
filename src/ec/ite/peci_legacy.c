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

// RSTC4
#define RPECI           BIT(4)  // Reset PECI module

// GCR2
#define PECIE           BIT(4)  // PECI Enable

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

// Part of message common to all functions
// XXX: Just make part of the request data?
struct PeciMsg {
    uint8_t addr;
    uint8_t wr_len;
    uint8_t rd_len;
    // part of data
    enum PeciCmd cmd;
};

static struct PeciMsg peci_msg;

uint8_t wr_dat[16] = {0};
uint8_t rd_dat[16] = {0};

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
static int8_t peci_transaction(void) {
    int8_t err;

    err = peci_wait_busy();
    if (err) {
        WARN("PECI host busy\n");
        return err;
    }

    HOCTLR |= FIFOCLR | FCSERR_ABT | PECIHEN | CONCTRL | AWFCS_EN;

    HOTRADDR = peci_msg.addr;
    HOWRLR = peci_msg.wr_len;
    HORDLR = peci_msg.rd_len;
    HOCMDR = peci_msg.cmd;

    // Write in WR_DAT FIFO bytes
    if (peci_msg.wr_len > 1) {
        for (uint8_t i = 0; i < peci_msg.wr_len - 1; i++) {
            HOWRDR = wr_dat[i];
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

    // Read out RD_DAT FIFO bytes
    for (uint8_t i = 0; i < peci_msg.rd_len; i++) {
        rd_dat[i] = HORDDR;
    }

    peci_clear_status();
    return 0;
}

int8_t peci_ping(void) {
    peci_msg.addr = PECI_CPU_ADDR;
    peci_msg.wr_len = PECI_PING_WR_LEN;
    peci_msg.rd_len = PECI_PING_RD_LEN;
    peci_msg.cmd = PECI_CMD_PING;

    int8_t err = peci_transaction();
    if (err) {
        WARN("PECI Ping() error %d\n", err);
        return err;
    }

    // Legacy Ping() returns 1 byte non-zero Write FCS if device is available.
    if (RWFCSV == 0) {
        return -ENODEV;
    }

    return 0;
}

int8_t peci_get_dib(void) {
    peci_msg.addr = PECI_CPU_ADDR;
    peci_msg.wr_len = PECI_GET_DIB_WR_LEN;
    peci_msg.rd_len = PECI_GET_DIB_RD_LEN;
    peci_msg.cmd = PECI_CMD_GET_DIB;

    int8_t err = peci_transaction();
    if (err) {
        WARN("PECI GetDIB() error: %d\n", err);
        return err;
    }

    // TODO: handle response
    return -EINVAL;
}

int8_t peci_rd_pkg_config(uint8_t index, uint16_t param, uint8_t rd_len) {
    peci_msg.addr = PECI_CPU_ADDR;
    peci_msg.wr_len = PECI_RD_PKG_WR_LEN;
    peci_msg.rd_len = rd_len;
    peci_msg.cmd = PECI_CMD_RD_PKG_CONFIG;

    wr_dat[0] = 0; // host/retry
    wr_dat[1] = index;
    wr_dat[2] = param & 0xFF;
    wr_dat[3] = (param >> 8) & 0xFF;

    int8_t err = peci_transaction();
    if (err) {
        WARN("PECI RdPkgConfig() error: %d\n", err);
        return err;
    }

    uint8_t cc = rd_dat[0];
    switch (cc) {
    case PECI_CC_RSP_SUCCESS:
        return 0;
    case PECI_CC_RSP_TIMEOUT:
        return -ETIMEDOUT;
    case PECI_CC_RSP_OUT_OF_RESOURCES:
        return -EAGAIN;
    default:
        return -EIO;
    }
}

int8_t peci_get_tjmax(uint8_t *max) {
    int8_t err = peci_rd_pkg_config(16, 0, PECI_RD_PKG_RD_LEN_4);
    if (err) {
        return err;
    }

    // rd_dat[1] = Tcontrol
    *max = rd_dat[2];

    return 0;
}

int8_t peci_wr_pkg_config(uint8_t index, uint16_t param, uint8_t wr_len) {
    peci_msg.addr = PECI_CPU_ADDR;
    peci_msg.wr_len = wr_len;
    peci_msg.rd_len = PECI_WR_PKG_RD_LEN;
    peci_msg.cmd = PECI_CMD_WR_PKG_CONFIG;

    wr_dat[0] = 0; // host/retry
    wr_dat[1] = index;
    wr_dat[2] = param & 0xFF;
    wr_dat[3] = (param >> 8) & 0xFF;

    int8_t err = peci_transaction();
    if (err) {
        WARN("PECI WrPkgConfig() error: %d\n", err);
        return err;
    }

    uint8_t cc = rd_dat[0];
    switch (cc) {
    case PECI_CC_RSP_SUCCESS:
        return 0;
    case PECI_CC_RSP_TIMEOUT:
        return -ETIMEDOUT;
    case PECI_CC_RSP_OUT_OF_RESOURCES:
        return -EAGAIN;
    default:
        return -EIO;
    }
}

int8_t peci_rd_aimsr(uint8_t processor, uint16_t msr, uint8_t rd_len) {
    peci_msg.addr = PECI_CPU_ADDR;
    peci_msg.wr_len = PECI_RD_IAMSR_WR_LEN;
    peci_msg.rd_len = rd_len;
    peci_msg.cmd = PECI_CMD_RD_IAMSR;

    wr_dat[0] = 0; // host/retry
    wr_dat[1] = processor;
    wr_dat[2] = msr & 0xFF;
    wr_dat[3] = (msr >> 8) & 0xFF;

    int8_t err = peci_transaction();
    if (err) {
        WARN("PECI RdIAMSR() error: %d\n", err);
        return err;
    }

    // TODO: handle response
    return 0;
}

int8_t peci_get_temp(int16_t *const temp) {
    peci_msg.addr = PECI_CPU_ADDR;
    peci_msg.wr_len = PECI_GET_TEMP_WR_LEN;
    peci_msg.rd_len = PECI_GET_TEMP_RD_LEN;
    peci_msg.cmd = PECI_CMD_GET_TEMP;

    int8_t err = peci_transaction();
    if (err) {
        WARN("PECI GetTemp() error: %d\n", err);
        return err;
    }

    int16_t offset = (rd_dat[1] << 8) | rd_dat[0];

    // XXX: Only need to read Tjmax once?
    uint8_t tjmax = 0;
    err = peci_get_tjmax(&tjmax);
    if (err || (tjmax == 0)) {
        return err;
    }

    // Discard the franctional value.
    *temp = tjmax + (offset >> 6);

    return 0;
}

int8_t peci_set_pl4(uint16_t watts) {
    uint32_t pl4 = (uint32_t)watts * 8;

    wr_dat[4] = pl4 & 0xFF;
    wr_dat[5] = (pl4 >> 8) & 0xFF;
    wr_dat[6] = (pl4 >> 16) & 0xFF;
    wr_dat[7] = (pl4 >> 24) & 0xFF;

    return peci_wr_pkg_config(60, 0, PECI_WR_PKG_WR_LEN_4);
}

void peci_init(void) {
    // NOTE: GPF6 set to ALT mode in board GPIO
    GCR2 |= PECIE;

    HOCTL2R = HOPTTRS_1MHZ;
    PADCTLR = HOVTTS_1P00V;
}
