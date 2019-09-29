#ifndef _PMC_H_
#define _PMC_H_

void pmc_init(void);

struct Pmc {
    // Status register
    volatile unsigned char * status;
    // Data out register
    volatile unsigned char * data_out;
    // Data in register
    volatile unsigned char * data_in;
    // Control register
    volatile unsigned char * control;
};

extern __code struct Pmc PMC_1;
extern __code struct Pmc PMC_2;

#define PMC_STS_OBF (1 << 0)
#define PMC_STS_IBF (1 << 1)
#define PMC_STS_CMD (1 << 3)

unsigned char pmc_status(struct Pmc * pmc);
unsigned char pmc_read(struct Pmc * pmc);
void pmc_write(struct Pmc * pmc, unsigned char data);

__xdata volatile unsigned char __at(0x1500) PM1STS;
__xdata volatile unsigned char __at(0x1501) PM1DO;
__xdata volatile unsigned char __at(0x1504) PM1DI;
__xdata volatile unsigned char __at(0x1506) PM1CTL;

__xdata volatile unsigned char __at(0x1510) PM2STS;
__xdata volatile unsigned char __at(0x1511) PM2DO;
__xdata volatile unsigned char __at(0x1514) PM2DI;
__xdata volatile unsigned char __at(0x1516) PM2CTL;

#endif // _PMC_H_
