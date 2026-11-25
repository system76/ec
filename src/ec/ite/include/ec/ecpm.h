// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_ECPM_H
#define _EC_ECPM_H

#include <common/macro.h>
#include <stdint.h>

volatile uint8_t __xdata __at(0x1E01) CGCTRL1;
volatile uint8_t __xdata __at(0x1E02) CGCTRL2;
#define SWUCCG          BIT(4)  // SWUC Clock Gating
#define CIRCG           BIT(5)  // CIR Clock Gating
#define EXCGC           BIT(6)  // EGPC Clock Gating

volatile uint8_t __xdata __at(0x1E03) PLLCTRL;
volatile uint8_t __xdata __at(0x1E04) AUTOCG;
volatile uint8_t __xdata __at(0x1E05) CGCTRL3;
#define DBGRCG          BIT(0)  // DBGR Clock Gating
#define SSPICG          BIT(1)  // SSPI Clock Gating
#define UART12CG        BIT(2)  // UART Clock Gating
#define PECICG          BIT(3)  // PECI Clock Gating
#define CGCTRL3_REQ     BIT(6)  // Reserved: "Always write 1 to this bit"

volatile uint8_t __xdata __at(0x1E06) PLLFREQ;
volatile uint8_t __xdata __at(0x1E08) PLLCSS;
volatile uint8_t __xdata __at(0x1E09) CGCTRL4;
#define CECCG           BIT(0)  // CEC Clock Gating

#endif // _EC_ECPM_H
