// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

#ifndef _BOARD_CONFIG_H
#define _BOARD_CONFIG_H

#include <stdbool.h>

bool config_should_reset(void);
void config_reset(void);

#endif // _BOARD_CONFIG_H
