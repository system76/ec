// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 System76, Inc.

// PECI over eSPI implementation
// Uses eSPI Out-of-Band (OOB) message channel.
//
// Requires:
// - `CONFIG_PLATFORM_INTEL`: Intel proprietary interface
// - `CONFIG_PECI_OVER_ESPI` (`CONFIG_BUS_ESPI`)

#include <drivers/peci/peci.h>
#include <arch/time.h>
#include <common/debug.h>
#include <common/errno.h>
#include <common/macro.h>
#include <ec/ecpm.h>
#include <ec/espi.h>
#include <ec/gpio.h>

struct PeciReq peci_req = { { 0 } };
struct PeciResp peci_resp = { { 0 } };

// SMBus header: Dest + Command Code + Byte Count bytes
#define SMBUS_HDR_LEN 3
// Protocol header: OOB Src + PECI Target + WR_LEN + RD_LEN bytes
#define PECI_REQ_HDR_LEN 4
// SMBus header + OOB Src + PECI Response/Error Status
#define OOB_MIN_RESP_LEN (SMBUS_HDR_LEN + 2)

// Command code
#define OOB_CMD_PECI    0x01
// Source/Dest slave addresses
#define OOB_EC_ADDR     0x0F
#define OOB_PCH_ADDR    0x10

#define OOB_DEST(addr)  (((addr) << 1) | 0)
#define OOB_SRC(addr)   (((addr) << 1) | 1)

// SMBus Packet Error Checking (PEC) CRC-8 table
static const uint8_t __code SMBUS_PEC[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3,
};

// Calculate PECI assured write frame check sequence (AW FCS) byte
static uint8_t peci_calc_awfcs(uint8_t *buffer, uint8_t len) {
    uint8_t awfcs = 0;

    for (uint8_t i = 0; i < len; i++) {
        awfcs ^= buffer[i];
        awfcs = SMBUS_PEC[awfcs];
    }

    return awfcs;
}

static int8_t peci_wait_ch_ready(void) {
    systick_t start = time_get();

    while (!(ESC2CAC0 & CH_OOB_READY)) {
        if ((time_get() - start) > PECI_TIMEOUT_MS) {
            return -EBUSY;
        }
    }

    return 0;
}

static int8_t peci_wait_busy(void) {
    systick_t start = time_get();

    while (ESUCTRL0 & ESUCTRL0_BUSY) {
        if ((time_get() - start) > PECI_TIMEOUT_MS) {
            return -EBUSY;
        }
    }

    return 0;
}

static int8_t peci_wait_send(void) {
    systick_t start = time_get();

    while (!(ESUCTRL0 & ESUCTRL0_DONE)) {
        if ((time_get() - start) > PECI_TIMEOUT_MS) {
            return -ETIMEDOUT;
        }
    }

    return 0;
}

static int peci_wait_recv(void) {
    systick_t start = time_get();

    while (!(ESOCTRL0 & ESOCTRL0_STATUS)) {
        if ((time_get() - start) > PECI_TIMEOUT_MS) {
            return -ETIMEDOUT;
        }
    }

    return 0;
}

// TODO: Allow retry for supported functions; Retry bit should be set.
int8_t peci_transaction(void) {
    uint8_t byte_count = peci_req.wr_len + PECI_REQ_HDR_LEN;
    int8_t err;

    // XXX: Is this check valid?
    err = peci_wait_ch_ready();
    if (err) {
        WARN("PECI eSPI OOB channel not ready\n");
        return err;
    }

    err = peci_wait_busy();
    if (err) {
        WARN("PECI host busy\n");
        return err;
    }

    // eSPI OOB header
    ESUCTRL1 = ESUCTRL1_OOB;
    ESUCTRL2 = 0;
    // XXX: Include PEC byte (check ESGCAC3[7])?
    ESUCTRL3 = byte_count + SMBUS_HDR_LEN;
    // SMBus header
    UDB[0] = OOB_DEST(OOB_PCH_ADDR);
    UDB[1] = OOB_CMD_PECI;
    UDB[2] = byte_count;
    // Protocol header
    UDB[3] = OOB_SRC(OOB_EC_ADDR);
    UDB[4] = peci_req.addr;
    UDB[5] = peci_req.wr_len;
    UDB[6] = peci_req.rd_len;
    // Protocol data
    // XXX: Ping() doesn't write command as a data byte. For convenience: Does it matter?
    UDB[7] = peci_req.cmd;

    if (peci_req.wr_len > 1) {
        for (uint8_t i = 0; i < peci_req.wr_len - 1; i++) {
            UDB[8 + i] = peci_req.data[i];
        }
    }

#if 0
    // TODO: AW FCS for WrPkgConfig()
    if (peci_req.cmd == PECI_CMD_WR_PKG_CONFIG) {
        UDB[_] = peci_calc_awfcs(_, _);
    }
#endif

    // Datasheet shows these should be set separately.
    ESUCTRL0 |= ESUCTRL0_ENABLE;
    ESUCTRL0 |= ESUCTRL0_GO;

    err = peci_wait_send();
    if (err) {
        WARN("PECI request timed out\n");
        // Disable upstream and clear done
        ESUCTRL0 = ESUCTRL0_DONE;
        return err;
    }

    // Disable upstream and clear done
    ESUCTRL0 = ESUCTRL0_DONE;

    err = peci_wait_recv();
    if (err) {
        WARN("PECI response timed out\n");
        // XXX: PUT_OOB wasn't set in time. Is it still valid to clear?
        goto recv_err;
    }

    uint8_t oob_len = ESOCTRL4;
    if (oob_len < OOB_MIN_RESP_LEN) {
        WARN("PECI OOB data too small: %u < %u\n", oob_len, OOB_MIN_RESP_LEN);
        err = -EIO;
        goto recv_err;
    }

    // Check message is for us.
    if (PUTOOBDB[0] != OOB_DEST(OOB_EC_ADDR)) {
        WARN("eSPI OOB message not for EC\n");
        err = -EFAULT;
        goto recv_err;
    }

    if (PUTOOBDB[1] != OOB_CMD_PECI) {
        WARN("eSPI OOB message is not PECI\n");
        err = -EIO;
        goto recv_err;
    }

    // TODO: Determine if this check is valid
    byte_count = PUTOOBDB[2];
    if (byte_count != peci_req.rd_len) {
        WARN("eSPI OOB message rd_len mismatch\n");
        err = -EIO;
        goto recv_err;
    }

    // TODO: How to check "PECI Response/Error Status"?
    //uint8_t peci_status = PUTOOBDB[4];

    for (uint8_t i = 0; i < byte_count; i++) {
        peci_resp.data[i] = PUTOOBDB[5 + i];
    }

    // Clear PUT_OOB to allow next incoming transaction
    ESOCTRL0 |= ESOCTRL0_STATUS;
    return 0;

recv_err:
    // Clear PUT_OOB to allow next incoming transaction
    ESOCTRL0 |= ESOCTRL0_STATUS;
    return err;
}

void peci_init(void) {
    // PECI module not used with eSPI
    // NOTE: If board has PECI wire, it must be configured in its GPIO.
    GCR2 &= ~PECIE;
    GCR3 |= PECIPDG;
    CGCTRL3 |= CGCTRL3_REQ | PECICG;
}
