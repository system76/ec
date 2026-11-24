// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 System76, Inc.

#include "peci.h"
#include <common/debug.h>
#include <common/errno.h>

// CPU Thermal and Power Optimization Services
#define CAP_TEMP_TARGET_READ    16
#define CAP_PKG_PL4             60

int8_t peci_ping(void) {
    peci_req.addr = PECI_CPU_ADDR;
    peci_req.wr_len = PECI_PING_WR_LEN;
    peci_req.rd_len = PECI_PING_RD_LEN;
    peci_req.cmd = PECI_CMD_PING;

    int8_t err = peci_transaction();
    if (err) {
        WARN("PECI Ping() error %d\n", err);
        return err;
    }

#if CONFIG_PECI_OVER_ESPI
    // Enhanced Ping() returns 2 bytes:
    // - [0]: 0x00
    // - [1]: 0x00=CPU is awake, 0x02=CPU is in reset
    if (peci_resp.data[0] == 0) {
        if (peci_resp.data[1] == 0) {
            return 0;
        } else if (peci_resp.data[1] == 2) {
            return -ENODEV;
        }
    }

    return -EIO;
#else
    // Legacy Ping() returns 1 byte non-zero Write FCS if device is available.
    // SoC logic must save it to the response buffer so it can be checked here.
    if (peci_resp.data[0] == 0) {
        return -ENODEV;
    }

    return 0;
#endif
}

int8_t peci_get_dib(void) {
    peci_req.addr = PECI_CPU_ADDR;
    peci_req.wr_len = PECI_GET_DIB_WR_LEN;
    peci_req.rd_len = PECI_GET_DIB_RD_LEN;
    peci_req.cmd = PECI_CMD_GET_DIB;

    int8_t err = peci_transaction();
    if (err) {
        WARN("PECI GetDIB() error: %d\n", err);
        return err;
    }

    return 0;
}

int8_t peci_rd_pkg_config(uint8_t index, uint16_t param, uint8_t rd_len) {
    peci_req.addr = PECI_CPU_ADDR;
    peci_req.wr_len = PECI_RD_PKG_WR_LEN;
    peci_req.rd_len = rd_len;
    peci_req.cmd = PECI_CMD_RD_PKG_CONFIG;

    peci_req.data[0] = 0; // host/retry
    peci_req.data[1] = index;
    peci_req.data[2] = param & 0xFF;
    peci_req.data[3] = (param >> 8) & 0xFF;

    int8_t err = peci_transaction();
    if (err) {
        WARN("PECI RdPkgConfig() error: %d\n", err);
        return err;
    }

    uint8_t cc = peci_resp.data[0];
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

int8_t peci_wr_pkg_config(uint8_t index, uint16_t param, uint8_t wr_len) {
    peci_req.addr = PECI_CPU_ADDR;
    peci_req.wr_len = wr_len;
    peci_req.rd_len = PECI_WR_PKG_RD_LEN;
    peci_req.cmd = PECI_CMD_WR_PKG_CONFIG;

    peci_req.data[0] = 0; // host/retry
    peci_req.data[1] = index;
    peci_req.data[2] = param & 0xFF;
    peci_req.data[3] = (param >> 8) & 0xFF;

    // AW FCS is handled by SoC logic.
    int8_t err = peci_transaction();
    if (err) {
        WARN("PECI WrPkgConfig() error: %d\n", err);
        return err;
    }

    uint8_t cc = peci_resp.data[0];
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
    peci_req.addr = PECI_CPU_ADDR;
    peci_req.wr_len = PECI_RD_IAMSR_WR_LEN;
    peci_req.rd_len = rd_len;
    peci_req.cmd = PECI_CMD_RD_IAMSR;

    peci_req.data[0] = 0; // host/retry
    peci_req.data[1] = processor;
    peci_req.data[2] = msr & 0xFF;
    peci_req.data[3] = (msr >> 8) & 0xFF;

    int8_t err = peci_transaction();
    if (err) {
        WARN("PECI RdIAMSR() error: %d\n", err);
        return err;
    }

    uint8_t cc = peci_resp.data[0];
    switch (cc) {
    case PECI_CC_RSP_SUCCESS:
        return 0;
    case PECI_CC_RSP_TIMEOUT:
        return -ETIMEDOUT;
    case PECI_CC_RSP_OUT_OF_RESOURCES:
    case PECI_CC_RSP_LOW_POWER_STATE:
        return -EAGAIN;
    default:
        return -EIO;
    }
}

static int8_t peci_get_tjmax(uint8_t *max) {
    int8_t err = peci_rd_pkg_config(CAP_TEMP_TARGET_READ, 0, PECI_RD_PKG_RD_LEN_4);
    if (err) {
        return err;
    }

    // peci_resp.data[1] is Tcontrol
    *max = peci_resp.data[2];

    return 0;
}

int8_t peci_get_temp(int16_t *const temp) {
    peci_req.addr = PECI_CPU_ADDR;
    peci_req.wr_len = PECI_GET_TEMP_WR_LEN;
    peci_req.rd_len = PECI_GET_TEMP_RD_LEN;
    peci_req.cmd = PECI_CMD_GET_TEMP;

    int8_t err = peci_transaction();
    if (err) {
        WARN("PECI GetTemp() error: %d\n", err);
        return err;
    }

    int16_t offset = (peci_resp.data[1] << 8) | peci_resp.data[0];

    static uint8_t tjmax = 0;
    if (tjmax == 0) {
        err = peci_get_tjmax(&tjmax);
        if (err) {
            WARN("PECI failed to get Tjmax: %d\n", err);
            return err;
        }
    }

    // Discard the fractional value.
    *temp = tjmax + (offset >> 6);

    return 0;
}

int8_t peci_set_pl4(uint16_t watts) {
    uint32_t pl4 = (uint32_t)watts * 8;

    peci_req.data[4] = pl4 & 0xFF;
    peci_req.data[5] = (pl4 >> 8) & 0xFF;
    peci_req.data[6] = (pl4 >> 16) & 0xFF;
    peci_req.data[7] = (pl4 >> 24) & 0xFF;

    return peci_wr_pkg_config(CAP_PKG_PL4, 0, PECI_WR_PKG_WR_LEN_4);
}
