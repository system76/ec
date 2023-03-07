// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_ESPI_H
#define _BOARD_ESPI_H

#include <stdbool.h>

#include <ec/espi.h>

extern bool espi_host_reset;

void espi_init(void);
void espi_reset(void);
void espi_event(void);

#endif // _BOARD_ESPI_H
