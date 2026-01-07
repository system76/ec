// SPDX-License-Identifier: GPL-3.0-only

#ifndef _APP_LID_H
#define _APP_LID_H

#include <stdbool.h>

extern bool lid_state;
extern bool lid_wake;

void lid_event(void);

#endif // _APP_LID_H
