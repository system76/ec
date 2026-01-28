// SPDX-License-Identifier: GPL-3.0-only

#ifndef _SOC_PMC_H
#define _SOC_PMC_H

#include <common/macro.h>

#include <stdbool.h>
#include <stdint.h>

struct Pmc {
    // Status register
    volatile uint8_t *status;
    // Data out register
    volatile uint8_t *data_out;
    // Data in register
    volatile uint8_t *data_in;
    // Control register
    volatile uint8_t *control;
    // Interrupt control register
    volatile uint8_t *interrupt_control;
    // Interrupt enable register
    volatile uint8_t *interrupt_enable;
};

extern struct Pmc __code PMC_1;
extern struct Pmc __code PMC_2;
extern struct Pmc __code PMC_3;
extern struct Pmc __code PMC_4;
extern struct Pmc __code PMC_5;

#define PMC_STS_OBF BIT(0)
#define PMC_STS_IBF BIT(1)
#define PMC_STS_CMD BIT(3)

uint8_t pmc_status(struct Pmc *const pmc);
void pmc_set_status(struct Pmc *const pmc, uint8_t status);
uint8_t pmc_read(struct Pmc *const pmc);
void pmc_write(struct Pmc *const pmc, uint8_t data);

volatile uint8_t __xdata __at(0x1500) PM1STS;
volatile uint8_t __xdata __at(0x1501) PM1DO;
volatile uint8_t __xdata __at(0x1502) PM1DOSCI;
volatile uint8_t __xdata __at(0x1503) PM1DOSMI;
volatile uint8_t __xdata __at(0x1504) PM1DI;
volatile uint8_t __xdata __at(0x1505) PM1DISCI;
volatile uint8_t __xdata __at(0x1506) PM1CTL;
volatile uint8_t __xdata __at(0x1507) PM1IC;
volatile uint8_t __xdata __at(0x1508) PM1IE;

volatile uint8_t __xdata __at(0x1510) PM2STS;
volatile uint8_t __xdata __at(0x1511) PM2DO;
volatile uint8_t __xdata __at(0x1514) PM2DI;
volatile uint8_t __xdata __at(0x1516) PM2CTL;
volatile uint8_t __xdata __at(0x1517) PM2IC;
volatile uint8_t __xdata __at(0x1518) PM2IE;

volatile uint8_t __xdata __at(0x1520) PM3STS;
volatile uint8_t __xdata __at(0x1521) PM3DO;
volatile uint8_t __xdata __at(0x1522) PM3DI;
volatile uint8_t __xdata __at(0x1523) PM3CTL;
volatile uint8_t __xdata __at(0x1524) PM3IC;
volatile uint8_t __xdata __at(0x1525) PM3IE;

volatile uint8_t __xdata __at(0x1530) PM4STS;
volatile uint8_t __xdata __at(0x1531) PM4DO;
volatile uint8_t __xdata __at(0x1532) PM4DI;
volatile uint8_t __xdata __at(0x1533) PM4CTL;
volatile uint8_t __xdata __at(0x1534) PM4IC;
volatile uint8_t __xdata __at(0x1535) PM4IE;

volatile uint8_t __xdata __at(0x1540) PM5STS;
volatile uint8_t __xdata __at(0x1541) PM5DO;
volatile uint8_t __xdata __at(0x1542) PM5DI;
volatile uint8_t __xdata __at(0x1543) PM5CTL;
volatile uint8_t __xdata __at(0x1544) PM5IC;
volatile uint8_t __xdata __at(0x1545) PM5IE;

#endif // _SOC_PMC_H
