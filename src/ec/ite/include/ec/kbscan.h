// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_KBSCAN_H
#define _EC_KBSCAN_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x1D00) KSOL;
volatile uint8_t __xdata __at(0x1D01) KSOH1;
volatile uint8_t __xdata __at(0x1D02) KSOCTRL;
volatile uint8_t __xdata __at(0x1D03) KSOH2;
volatile uint8_t __xdata __at(0x1D04) KSI;
volatile uint8_t __xdata __at(0x1D05) KSICTRLR;
volatile uint8_t __xdata __at(0x1D06) KSIGCTRL;
volatile uint8_t __xdata __at(0x1D07) KSIGOEN;
volatile uint8_t __xdata __at(0x1D08) KSIGDAT;
volatile uint8_t __xdata __at(0x1D09) KSIGDMRR;
volatile uint8_t __xdata __at(0x1D0A) KSOHGCTRL;
volatile uint8_t __xdata __at(0x1D0B) KSOHGOEN;
volatile uint8_t __xdata __at(0x1D0C) KSOHGDMRR;
volatile uint8_t __xdata __at(0x1D0D) KSOLGCTRL;
volatile uint8_t __xdata __at(0x1D0E) KSOLGOEN;
volatile uint8_t __xdata __at(0x1D0F) KSOLGDMRR;

volatile uint8_t __xdata __at(0x1D10) KSO0LSDR;
volatile uint8_t __xdata __at(0x1D11) KSO1LSDR;
volatile uint8_t __xdata __at(0x1D12) KSO2LSDR;
volatile uint8_t __xdata __at(0x1D13) KSO3LSDR;
volatile uint8_t __xdata __at(0x1D14) KSO4LSDR;
volatile uint8_t __xdata __at(0x1D15) KSO5LSDR;
volatile uint8_t __xdata __at(0x1D16) KSO6LSDR;
volatile uint8_t __xdata __at(0x1D17) KSO7LSDR;
volatile uint8_t __xdata __at(0x1D18) KSO8LSDR;
volatile uint8_t __xdata __at(0x1D19) KSO9LSDR;
volatile uint8_t __xdata __at(0x1D1A) KSO10LSDR;
volatile uint8_t __xdata __at(0x1D1B) KSO11LSDR;
volatile uint8_t __xdata __at(0x1D1C) KSO12LSDR;
volatile uint8_t __xdata __at(0x1D1D) KSO13LSDR;
volatile uint8_t __xdata __at(0x1D1E) KSO14LSDR;
volatile uint8_t __xdata __at(0x1D1F) KSO15LSDR;
volatile uint8_t __xdata __at(0x1D20) KSO16LSDR;
volatile uint8_t __xdata __at(0x1D21) KSO17LSDR;

volatile uint8_t __xdata __at(0x1D22) SDC1R;
volatile uint8_t __xdata __at(0x1D23) SDC2R;
volatile uint8_t __xdata __at(0x1D24) SDC3R;
volatile uint8_t __xdata __at(0x1D25) SDSR;

#endif // _EC_KBSCAN_H
