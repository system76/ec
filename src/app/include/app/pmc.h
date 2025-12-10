// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2019 System76, Inc.

#ifndef _APP_PMC_H
#define _APP_PMC_H

#include <soc/pmc.h>

void pmc_init(void);
bool pmc_sci(struct Pmc *const pmc, uint8_t sci);
void pmc_swi(void);
void pmc_event(struct Pmc *const pmc);

#endif // _APP_PMC_H
