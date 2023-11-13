// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_POWER_H
#define _BOARD_POWER_H

#include <stdbool.h>

enum PowerState {
    POWER_STATE_OFF,
    POWER_STATE_S5,
    POWER_STATE_S3,
    POWER_STATE_S0,
};

extern enum PowerState power_state;
void update_power_state(void);

#if USE_S0IX
extern bool pep_in_s0ix;
extern bool pep_display_on;
#endif

void power_init(void);
void power_on(void);
void power_off(void);
void power_cpu_reset(void);

void power_event(void);

bool power_peci_limit(bool);

#endif // _BOARD_POWER_H
