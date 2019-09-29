#ifndef _GCTRL_H_
#define _GCTRL_H_

void gctrl_init(void);

__xdata volatile unsigned char __at(0x2006) RSTS;
__xdata volatile unsigned char __at(0x200A) BADRSEL;
__xdata volatile unsigned char __at(0x200D) SPCTRL1;

#endif // _GCTRL_H_
