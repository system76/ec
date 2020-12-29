// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_ESPI_H
#define _EC_ESPI_H
#include <stdint.h>

#ifndef EC_ESPI
#define EC_ESPI 0
#endif // EC_ESPI

struct VirtualWire {
    volatile uint8_t __xdata * index;
    uint8_t shift;
};

#define VIRTUAL_WIRE(INDEX, SHIFT) { \
    .index = &VWIDX ## INDEX, \
    .shift = SHIFT, \
}

enum VirtualWireState {
    VWS_INVALID = 0x00,
    VWS_LOW = 0x10,
    VWS_HIGH = 0x11,
};

enum VirtualWireState vw_get(struct VirtualWire * vw) __critical;

void vw_set(struct VirtualWire * vw, enum VirtualWireState state) __critical;

// Not all wires are defined or implemented
// Index 2 - AP to EC
extern struct VirtualWire __code VW_SLP_S3_N;
extern struct VirtualWire __code VW_SLP_S4_N;
extern struct VirtualWire __code VW_SLP_S5_N;
// Index 3 - AP to EC
extern struct VirtualWire __code VW_SUS_STAT_N;
extern struct VirtualWire __code VW_PLTRST_N;
extern struct VirtualWire __code VW_OOB_RST_WARN;
// Index 4 - EC to AP
extern struct VirtualWire __code VW_OOB_RST_ACK;
extern struct VirtualWire __code VW_WAKE_N;
extern struct VirtualWire __code VW_PME_N;
// Index 5 - EC to AP
extern struct VirtualWire __code VW_BOOT_LOAD_DONE;
extern struct VirtualWire __code VW_ERROR_FATAL;
extern struct VirtualWire __code VW_ERROR_NONFATAL;
extern struct VirtualWire __code VW_BOOT_LOAD_STATUS;
// Index 6 - EC to AP
extern struct VirtualWire __code VW_SCI_N;
extern struct VirtualWire __code VW_SMI_N;
extern struct VirtualWire __code VW_RCIN_N;
extern struct VirtualWire __code VW_HOST_RST_ACK;
// Index 7 - AP to EC
extern struct VirtualWire __code VW_HOST_RST_WARN;
// Index 40 - EC to AP (platform specific)
extern struct VirtualWire __code VW_SUS_ACK_N;
// Index 41 - AP to EC (platform specific)
extern struct VirtualWire __code VW_SUS_WARN_N;
extern struct VirtualWire __code VW_SUS_PWRDN_ACK;

// General capabilities and configurations
volatile uint8_t __xdata __at(0x3107) ESGCAC0;
volatile uint8_t __xdata __at(0x3106) ESGCAC1;
volatile uint8_t __xdata __at(0x3105) ESGCAC2;
volatile uint8_t __xdata __at(0x3104) ESGCAC3;

// Channel 0 capabilities and configurations
volatile uint8_t __xdata __at(0x310B) ESC0CAC0;
volatile uint8_t __xdata __at(0x310A) ESC0CAC1;
volatile uint8_t __xdata __at(0x3109) ESC0CAC2;
volatile uint8_t __xdata __at(0x3108) ESC0CAC3;

// Channel 1 capabilities and configurations
volatile uint8_t __xdata __at(0x310F) ESC1CAC0;
volatile uint8_t __xdata __at(0x310E) ESC1CAC1;
volatile uint8_t __xdata __at(0x310D) ESC1CAC2;
volatile uint8_t __xdata __at(0x310C) ESC1CAC3;

// Channel 2 capabilities and configurations
volatile uint8_t __xdata __at(0x3113) ESC2CAC0;
volatile uint8_t __xdata __at(0x3112) ESC2CAC1;
volatile uint8_t __xdata __at(0x3111) ESC2CAC2;
volatile uint8_t __xdata __at(0x3110) ESC2CAC3;

// Channel 3 capabilities and configurations
volatile uint8_t __xdata __at(0x3117) ESC3CAC0;
volatile uint8_t __xdata __at(0x3116) ESC3CAC1;
volatile uint8_t __xdata __at(0x3115) ESC3CAC2;
volatile uint8_t __xdata __at(0x3114) ESC3CAC3;

// eSPI PC control
volatile uint8_t __xdata __at(0x3190) ESPCTRL0;

// eSPI general control
volatile uint8_t __xdata __at(0x31A0) ESGCTRL0;
volatile uint8_t __xdata __at(0x31A1) ESGCTRL1;
volatile uint8_t __xdata __at(0x31A2) ESGCTRL2;
volatile uint8_t __xdata __at(0x31A3) ESGCTRL3;

// Virtual wires
volatile uint8_t __xdata __at(0x3200) VWIDX0;
volatile uint8_t __xdata __at(0x3202) VWIDX2;
volatile uint8_t __xdata __at(0x3203) VWIDX3;
volatile uint8_t __xdata __at(0x3204) VWIDX4;
volatile uint8_t __xdata __at(0x3205) VWIDX5;
volatile uint8_t __xdata __at(0x3206) VWIDX6;
volatile uint8_t __xdata __at(0x3207) VWIDX7;

// Virtual wires at 0x40
volatile uint8_t __xdata __at(0x3240) VWIDX40;
volatile uint8_t __xdata __at(0x3241) VWIDX41;
volatile uint8_t __xdata __at(0x3242) VWIDX42;

// Virtual wire control
volatile uint8_t __xdata __at(0x3290) VWCTRL0;
volatile uint8_t __xdata __at(0x3291) VWCTRL1;
volatile uint8_t __xdata __at(0x3292) VWCTRL2;
volatile uint8_t __xdata __at(0x3293) VWCTRL3;
volatile uint8_t __xdata __at(0x3295) VWCTRL5;
volatile uint8_t __xdata __at(0x3296) VWCTRL6;

#endif // _EC_ESPI_H
