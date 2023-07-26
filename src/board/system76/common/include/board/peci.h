// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_PECI_H
#define _BOARD_PECI_H

#include <stdbool.h>

#include <ec/peci.h>
#include <board/fan.h>

#define PECI_REG_PKG_CFG_PL3 57
#define PECI_REG_PKG_CFG_PSYS_PL2 59
#define PECI_REG_PKG_CFG_PL4 60

#define PECI_PL_ENABLE BIT(15)
#define PECI_PL_CLAMP BIT(16)
#define PECI_PL_TIME_WINDOW(time, duty) \
    ((uint32_t)((time)&0x3f) << 17 | (uint32_t)((duty)&0xf) << 24)

#define PECI_PSYS_PL2(watts) ((watts)*8 | PECI_PL_ENABLE | PECI_PL_CLAMP)
#define PECI_PL3(watts, time, duty) \
    ((watts)*8 | PECI_PL_ENABLE | PECI_PL_CLAMP | PECI_PL_TIME_WINDOW(time, duty))
#define PECI_PL4(watts) ((watts)*8)

// Set the CPU power limit appropriately
extern bool peci_on;
extern int16_t peci_temp;

void peci_init(void);
bool peci_available(void);
int16_t peci_set_fan_curve(uint8_t count, struct FanPoint *points);
int16_t peci_wr_pkg_config(uint8_t index, uint16_t param, uint32_t data);
uint8_t peci_get_fan_duty(void);

#endif // _BOARD_PECI_H
