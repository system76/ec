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

#define PMC_STS_OBF (1 << 0)
#define PMC_STS_IBF (1 << 1)
#define PMC_STS_CMD (1 << 3)

uint8_t pmc_status(struct Pmc * pmc);
uint8_t pmc_read(struct Pmc * pmc);
bool pmc_write(struct Pmc * pmc, uint8_t data, int timeout);

volatile uint8_t __xdata __at(0x1500) PM1STS;
volatile uint8_t __xdata __at(0x1501) PM1DO;
volatile uint8_t __xdata __at(0x1504) PM1DI;
volatile uint8_t __xdata __at(0x1506) PM1CTL;

#endif // _EC_PMC_H
