// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_PECI_H
#define _BOARD_PECI_H

#include <stdbool.h>

#include <ec/peci.h>
#include <board/fan.h>

extern bool peci_on;
extern int16_t peci_temp;

void peci_init(void);
bool peci_available(void);
int16_t peci_set_fan_curve(uint8_t count, struct FanPoint * points);
int16_t peci_wr_pkg_config(uint8_t index, uint16_t param, uint32_t data);
uint8_t peci_get_fan_duty(void);

#endif // _BOARD_PECI_H
