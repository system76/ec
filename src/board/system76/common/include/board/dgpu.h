// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_DGPU_H
#define _BOARD_DGPU_H

#include <stdint.h>
#include <board/fan.h>

#ifndef HAVE_DGPU
#define HAVE_DGPU 0
#endif

#ifndef HAVE_D_NOTIFY
#define HAVE_D_NOTIFY 0
#endif

#if HAVE_DGPU
extern int16_t dgpu_temp;
#endif // HAVE_DGPU

void dgpu_init(void);
int16_t dgpu_set_fan_curve(uint8_t count, struct FanPoint *points);
uint8_t dgpu_get_fan_duty(void);
uint8_t dgpu_get_d_notify_level(bool ac);
void set_mux_ctrl(void);

#endif // _BOARD_DGPU_H
