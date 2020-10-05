// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_PECI_H
#define _BOARD_PECI_H

#include <ec/peci.h>

extern int16_t peci_temp;

void peci_init(void);
int peci_wr_pkg_config(uint8_t index, uint16_t param, uint32_t data);
void peci_event(void);

#endif // _BOARD_PECI_H
