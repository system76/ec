#ifndef _KBC_H_
#define _KBC_H_

__xdata volatile unsigned char __at(0x1300) KBHICR;
__xdata volatile unsigned char __at(0x1302) KBIRQR;
__xdata volatile unsigned char __at(0x1304) KBHISR;
__xdata volatile unsigned char __at(0x1306) KBHIKDOR;
__xdata volatile unsigned char __at(0x1308) KBHIMDOR;
__xdata volatile unsigned char __at(0x130A) KBHIDIR;

#endif // _KBC_H_
