#ifndef _PMC_H_
#define _PMC_H_

void pmc_init(void);

__xdata volatile unsigned char __at(0x1506) PM1CTL;
__xdata volatile unsigned char __at(0x1516) PM2CTL;

#endif // _PMC_H_
