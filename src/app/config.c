// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

#include <app/config.h>

#include <app/battery.h>
#include <app/kbscan.h>
#include <board/keymap.h>
#include <common/debug.h>

/**
 * Test if the EC should reset its configuration.
 */
bool config_should_reset(void) {
    return kbscan_fn_held && kbscan_esc_held;
}

/**
 * Reset the EC configuration data.
 */
void config_reset(void) {
    INFO("Reset configuration\n");
    battery_reset();
    keymap_erase_config();
    keymap_load_default();
}
