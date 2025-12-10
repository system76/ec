// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2019 System76, Inc.

#ifndef _APP_POWER_H
#define _APP_POWER_H

enum PowerState {
    POWER_STATE_OFF,
    POWER_STATE_S5,
    POWER_STATE_S3,
    POWER_STATE_S0,
};

extern enum PowerState power_state;
void update_power_state(void);

// SMBIOS Wake-up Type
enum PowerWakeupType {
    POWER_WAKEUP_TYPE_RESERVED = 0,
    POWER_WAKEUP_TYPE_OTHER = 1,
    POWER_WAKEUP_TYPE_UNKNOWN = 2,
    POWER_WAKEUP_TYPE_APM_TIMER = 3,
    POWER_WAKEUP_TYPE_MODEM_RING = 4,
    POWER_WAKEUP_TYPE_LAN_REMOTE = 5,
    POWER_WAKEUP_TYPE_POWER_SWITCH = 6,
    POWER_WAKEUP_TYPE_PCI_PME = 7,
    POWER_WAKEUP_TYPE_AC_POWER_RESTORED = 8,
};

extern enum PowerWakeupType power_wakeup_type;

void power_init(void);
void power_on(void);
void power_off(void);
void power_cpu_reset(void);
void power_event(void);

#endif // _APP_POWER_H
