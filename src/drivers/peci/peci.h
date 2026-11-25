// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 System76, Inc.

// Platform Environment Control Interface (PECI)

#ifndef _DRIVERS_PECI_PECI_H
#define _DRIVERS_PECI_PECI_H

#include <stdint.h>

enum PeciCmd {
    PECI_CMD_PING = 0x00,
    PECI_CMD_GET_TEMP = 0x01,
    PECI_CMD_RD_PKG_CONFIG = 0xA1,
    PECI_CMD_WR_PKG_CONFIG = 0xA5,
    PECI_CMD_RD_IAMSR = 0xB1,
    PECI_CMD_GET_DIB = 0xF7,
};

#define PECI_CPU_ADDR   0x30

// Maximum transaction completion time in ms
#define PECI_TIMEOUT_MS 10

// Completion codes
#define PECI_CC_RSP_SUCCESS             0x40
#define PECI_CC_RSP_TIMEOUT             0x80
#define PECI_CC_RSP_OUT_OF_RESOURCES    0x81
#define PECI_CC_RSP_LOW_POWER_STATE     0x82
#define PECI_CC_RSP_INVALID_REQUEST     0x90
#define PECI_CC_RSP_LOGIC_ERROR         0x91

// Ping
#define PECI_PING_WR_LEN        0
#define PECI_PING_RD_LEN        0
// GetDIB
#define PECI_GET_DIB_WR_LEN     1
#define PECI_GET_DIB_RD_LEN     8
// GetTemp
#define PECI_GET_TEMP_WR_LEN    1
#define PECI_GET_TEMP_RD_LEN    2
// WrPkgConfig
#define PECI_WR_PKG_WR_LEN_1    7
#define PECI_WR_PKG_WR_LEN_2    8
#define PECI_WR_PKG_WR_LEN_4    10
#define PECI_WR_PKG_RD_LEN      1
// RdPkgConfig
#define PECI_RD_PKG_WR_LEN      5
#define PECI_RD_PKG_RD_LEN_1    2
#define PECI_RD_PKG_RD_LEN_2    3
#define PECI_RD_PKG_RD_LEN_4    5
// RdIAMSR
#define PECI_RD_IAMSR_WR_LEN    5
#define PECI_RD_IAMSR_RD_LEN_1  2
#define PECI_RD_IAMSR_RD_LEN_2  3
#define PECI_RD_IAMSR_RD_LEN_4  5
#define PECI_RD_IAMSR_RD_LEN_8  9

struct PeciReq {
    uint8_t addr;
    uint8_t wr_len;
    uint8_t rd_len;
    // Technically part of the payload data.
    enum PeciCmd cmd;
    uint8_t data[16];
};

extern struct PeciReq peci_req;

struct PeciResp {
    uint8_t data[16];
};

extern struct PeciResp peci_resp;

int8_t peci_ping(void);
int8_t peci_get_dib(void);
int8_t peci_rd_pkg_config(uint8_t index, uint16_t param, uint8_t rd_len);
int8_t peci_wr_pkg_config(uint8_t index, uint16_t param, uint8_t wr_len);
int8_t peci_rd_aimsr(uint8_t processor, uint16_t msr, uint8_t rd_len);
int8_t peci_get_temp(int16_t *const temp);

int8_t peci_set_pl4(uint16_t watts);

// SoC-specific implementation functions
void peci_init(void);
int8_t peci_transaction(void);

#endif // _DRIVERS_PECI_PECI_H
