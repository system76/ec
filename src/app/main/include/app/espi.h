// SPDX-License-Identifier: GPL-3.0-only

#ifndef _APP_ESPI_H
#define _APP_ESPI_H

#include <stdbool.h>

#include <soc/espi.h>

extern bool espi_host_reset;

void espi_init(void);
void espi_reset(void);
void espi_event(void);

#endif // _APP_ESPI_H
