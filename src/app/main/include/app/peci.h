// SPDX-License-Identifier: GPL-3.0-only

#ifndef _APP_PECI_H
#define _APP_PECI_H

#include <drivers/peci/peci.h>
#include <ec/peci.h>
#include <stdbool.h>

extern int16_t peci_temp;

bool peci_available(void);
void peci_read_temp(void);

#endif // _APP_PECI_H
