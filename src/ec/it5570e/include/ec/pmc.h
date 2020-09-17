// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_PMC_H
#define _EC_PMC_H

#include <stdbool.h>
#include <stdint.h>

struct Pmc {
    // Status register
    volatile uint8_t * status;
    // Data out register
    volatile uint8_t * data_out;
    // Data in register
    volatile uint8_t * data_in;
    // Control register
    volatile uint8_t * control;
};

extern struct Pmc __code PMC_1;
extern struct Pmc __code PMC_2;
extern struct Pmc __code PMC_3;
extern struct Pmc __code PMC_4;
extern struct Pmc __code PMC_5;

#define PMC_STS_OBF (1 << 0)
#define PMC_STS_IBF (1 << 1)
#define PMC_STS_CMD (1 << 3)

uint8_t pmc_status(struct Pmc * pmc);
void pmc_set_status(struct Pmc * pmc, uint8_t status);
uint8_t pmc_read(struct Pmc * pmc);
void pmc_write(struct Pmc * pmc, uint8_t data);

volatile uint8_t __xdata __at(0x1500) PM1STS;
volatile uint8_t __xdata __at(0x1501) PM1DO;
volatile uint8_t __xdata __at(0x1504) PM1DI;
volatile uint8_t __xdata __at(0x1506) PM1CTL;

volatile uint8_t __xdata __at(0x1510) PM2STS;
volatile uint8_t __xdata __at(0x1511) PM2DO;
volatile uint8_t __xdata __at(0x1514) PM2DI;
volatile uint8_t __xdata __at(0x1516) PM2CTL;

volatile uint8_t __xdata __at(0x1520) PM3STS;
volatile uint8_t __xdata __at(0x1521) PM3DO;
volatile uint8_t __xdata __at(0x1522) PM3DI;
volatile uint8_t __xdata __at(0x1523) PM3CTL;

volatile uint8_t __xdata __at(0x1530) PM4STS;
volatile uint8_t __xdata __at(0x1531) PM4DO;
volatile uint8_t __xdata __at(0x1532) PM4DI;
volatile uint8_t __xdata __at(0x1533) PM4CTL;

volatile uint8_t __xdata __at(0x1540) PM5STS;
volatile uint8_t __xdata __at(0x1541) PM5DO;
volatile uint8_t __xdata __at(0x1542) PM5DI;
volatile uint8_t __xdata __at(0x1543) PM5CTL;

#endif // _EC_PMC_H
