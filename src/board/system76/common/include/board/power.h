// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_POWER_H
#define _BOARD_POWER_H

enum PowerState {
    POWER_STATE_OFF,
    POWER_STATE_S5,
    POWER_STATE_S3,
    POWER_STATE_S0,
};

extern enum PowerState power_state;

void power_init(void);
void power_on(void);
void power_off(void);
void power_set_limit(void);
void power_cpu_reset(void);

void power_event(void);

void power_handle_lan_wakeup(void);

#endif // _BOARD_POWER_H
