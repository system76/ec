// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

#ifndef _APP_SMFI_H
#define _APP_SMFI_H

#include <stdint.h>

void smfi_init(void);
void smfi_watchdog(void);
void smfi_event(void);
void smfi_debug(uint8_t byte);

#endif // _APP_SMFI_H
