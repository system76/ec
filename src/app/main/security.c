// SPDX-License-Identifier: GPL-3.0-only

#include <app/security.h>
#include <board/gpio.h>

static enum SecurityState security_state = SECURITY_STATE_LOCK;

enum SecurityState security_get(void) {
    return security_state;
}

bool security_set(enum SecurityState state) {
    switch (state) {
    // Allow prepare states to be set
    case SECURITY_STATE_PREPARE_LOCK:
    case SECURITY_STATE_PREPARE_UNLOCK:
        security_state = state;
        return true;
    // Any other states will be ignored
    default:
        return false;
    }
}

bool security_power(void) {
    switch (security_state) {
    // Apply lock state and power on
    case SECURITY_STATE_PREPARE_LOCK:
        gpio_set(&ME_WE, false);
        security_state = SECURITY_STATE_LOCK;
        return true;
    // Apply unlock state and power on
    case SECURITY_STATE_PREPARE_UNLOCK:
        gpio_set(&ME_WE, true);
        security_state = SECURITY_STATE_UNLOCK;
        return true;
    // Any other states will be ignored
    default:
        return false;
    }
}
