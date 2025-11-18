// SPDX-License-Identifier: GPL-3.0-only

#ifndef _DRIVERS_SECURITY_H
#define _DRIVERS_SECURITY_H

#include <stdbool.h>

enum SecurityState {
    // Default value, flashing is prevented, cannot be set with CMD_SECURITY_SET
    SECURITY_STATE_LOCK = 0,
    // Flashing is allowed, cannot be set with CMD_SECURITY_SET
    SECURITY_STATE_UNLOCK = 1,
    // Flashing will be prevented on the next reboot
    SECURITY_STATE_PREPARE_LOCK = 2,
    // Flashing will be allowed on the next reboot
    SECURITY_STATE_PREPARE_UNLOCK = 3,
};

enum SecurityState security_get(void);
bool security_set(enum SecurityState state);
bool security_power(void);

#endif // _DRIVERS_SECURITY_H
