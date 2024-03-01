// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_PMC_H
#define _BOARD_PMC_H

#include <ec/pmc.h>

void pmc_init(void);
bool pmc_sci(struct Pmc *const pmc, uint8_t sci);
void pmc_swi(void);
void pmc_event(struct Pmc *const pmc);

#endif // _BOARD_PMC_H
