#ifndef _EC_KBSCAN_H
#define _EC_KBSCAN_H

#include <stdint.h>

__xdata volatile uint8_t __at(0x1D00) KSOL;
__xdata volatile uint8_t __at(0x1D01) KSOH1;
__xdata volatile uint8_t __at(0x1D02) KSOCTRL;
__xdata volatile uint8_t __at(0x1D03) KSOH2;
__xdata volatile uint8_t __at(0x1D04) KSI;
__xdata volatile uint8_t __at(0x1D05) KSICTRLR;
__xdata volatile uint8_t __at(0x1D06) KSIGCTRL;
__xdata volatile uint8_t __at(0x1D07) KSIGOEN;
__xdata volatile uint8_t __at(0x1D08) KSIGDAT;
__xdata volatile uint8_t __at(0x1D09) KSIGDMRR;
__xdata volatile uint8_t __at(0x1D0A) KSOHGCTRL;
__xdata volatile uint8_t __at(0x1D0B) KSOHGOEN;
__xdata volatile uint8_t __at(0x1D0C) KSOHGDMRR;
__xdata volatile uint8_t __at(0x1D0D) KSOLGCTRL;
__xdata volatile uint8_t __at(0x1D0E) KSOLGOEN;
__xdata volatile uint8_t __at(0x1D0F) KSOLGDMRR;

__xdata volatile uint8_t __at(0x1D10) KSO0LSDR;
__xdata volatile uint8_t __at(0x1D11) KSO1LSDR;
__xdata volatile uint8_t __at(0x1D12) KSO2LSDR;
__xdata volatile uint8_t __at(0x1D13) KSO3LSDR;
__xdata volatile uint8_t __at(0x1D14) KSO4LSDR;
__xdata volatile uint8_t __at(0x1D15) KSO5LSDR;
__xdata volatile uint8_t __at(0x1D16) KSO6LSDR;
__xdata volatile uint8_t __at(0x1D17) KSO7LSDR;
__xdata volatile uint8_t __at(0x1D18) KSO8LSDR;
__xdata volatile uint8_t __at(0x1D19) KSO9LSDR;
__xdata volatile uint8_t __at(0x1D1A) KSO10LSDR;
__xdata volatile uint8_t __at(0x1D1B) KSO11LSDR;
__xdata volatile uint8_t __at(0x1D1C) KSO12LSDR;
__xdata volatile uint8_t __at(0x1D1D) KSO13LSDR;
__xdata volatile uint8_t __at(0x1D1E) KSO14LSDR;
__xdata volatile uint8_t __at(0x1D1F) KSO15LSDR;
__xdata volatile uint8_t __at(0x1D20) KSO16LSDR;
__xdata volatile uint8_t __at(0x1D21) KSO17LSDR;

__xdata volatile uint8_t __at(0x1D22) SDC1R;
__xdata volatile uint8_t __at(0x1D23) SDC2R;
__xdata volatile uint8_t __at(0x1D24) SDC3R;
__xdata volatile uint8_t __at(0x1D25) SDSR;

#endif // _EC_KBSCAN_H
