// SPDX-License-Identifier: GPL-3.0-only

#ifndef _APP_PECI_H
#define _APP_PECI_H

#include <ec/peci.h>
#include <stdbool.h>

extern bool peci_on;
extern int16_t peci_temp;

enum PeciCmd {
    PECI_CMD_PING = 0x00,
    PECI_CMD_GET_TEMP = 0x01,
    PECI_CMD_RD_PKG_CONFIG = 0xA1,
    PECI_CMD_WR_PKG_CONFIG = 0xA5,
    PECI_CMD_RD_IAMSR = 0xB1,
    PECI_CMD_GET_DIB = 0xF7,
};

void peci_init(void);
bool peci_available(void);
int16_t peci_wr_pkg_config(uint8_t index, uint16_t param, uint32_t data);
void peci_read_temp(void);

#endif // _APP_PECI_H
