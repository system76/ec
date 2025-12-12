// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

#ifndef _BOARD_LID_H
#define _BOARD_LID_H

#include <stdbool.h>

extern bool lid_state;
extern bool lid_wake;

void lid_event(void);

#endif // _BOARD_LID_H
