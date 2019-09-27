#ifndef _PS2_H_
#define _PS2_H_

void ps2_init(void);

__xdata volatile unsigned char __at(0x1700) PSCTL1;
__xdata volatile unsigned char __at(0x1701) PSCTL2;
__xdata volatile unsigned char __at(0x1702) PSCTL3;

__xdata volatile unsigned char __at(0x1704) PSINT1;
__xdata volatile unsigned char __at(0x1705) PSINT2;
__xdata volatile unsigned char __at(0x1706) PSINT3;

__xdata volatile unsigned char __at(0x1708) PSSTS1;
__xdata volatile unsigned char __at(0x1709) PSSTS2;
__xdata volatile unsigned char __at(0x170A) PSSTS3;

__xdata volatile unsigned char __at(0x170C) PSDAT1;
__xdata volatile unsigned char __at(0x170D) PSDAT2;
__xdata volatile unsigned char __at(0x170E) PSDAT3;

#endif // _PS2_H_
