// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2023 System76, Inc.

#ifndef _APP_SECURITY_H
#define _APP_SECURITY_H

#include <stdbool.h>
#include <common/command.h>

enum SecurityState security_get(void);
bool security_set(enum SecurityState state);
bool security_power(void);

#endif // _APP_SECURITY_H
