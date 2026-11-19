// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_PECI_H
#define _EC_PECI_H

#include <stdint.h>

// Host Status Register
static volatile uint8_t __xdata __at(0x3000) HOSTAR;
// Host Control Register
static volatile uint8_t __xdata __at(0x3001) HOCTLR;
// Host Command (Write 1) Register
static volatile uint8_t __xdata __at(0x3002) HOCMDR;
// Host Target Address Register
static volatile uint8_t __xdata __at(0x3003) HOTRADDR;
// Host Write Length Register
static volatile uint8_t __xdata __at(0x3004) HOWRLR;
// Host Read Length Register
static volatile uint8_t __xdata __at(0x3005) HORDLR;
// Host Write Data (2-16) Register (FIFO)
static volatile uint8_t __xdata __at(0x3006) HOWRDR;
// Host Read Data (1-16) Register (FIFO)
static volatile uint8_t __xdata __at(0x3007) HORDDR;
// Host Control 2 Register
static volatile uint8_t __xdata __at(0x3008) HOCTL2R;
// Received Write FCS Value
static volatile uint8_t __xdata __at(0x3009) RWFCSV;
// Received Read FCS Value
static volatile uint8_t __xdata __at(0x300A) RRFCSV;
// Write FCS Value
static volatile uint8_t __xdata __at(0x300B) WFCSV;
// Read FCS Value
static volatile uint8_t __xdata __at(0x300C) RFCSV;
// Assured Write FCS Value
static volatile uint8_t __xdata __at(0x300D) AWFCSV;
// Pad Control Register
static volatile uint8_t __xdata __at(0x300E) PADCTLR;

// TODO: Move functions/externs to driver

extern uint8_t wr_dat[16]; // Request data (WR_DAT)
extern uint8_t rd_dat[16]; // Response data (RD_DAT)

int8_t peci_ping(void);
int8_t peci_get_dib(void);
int8_t peci_rd_pkg_config(uint8_t index, uint16_t param, uint8_t rd_len);
int8_t peci_wr_pkg_config(uint8_t index, uint16_t param, uint8_t wr_len);
int8_t peci_rd_aimsr(uint8_t processor, uint16_t msr, uint8_t rd_len);
int8_t peci_get_temp(int16_t *const temp);

int8_t peci_set_pl4(uint16_t watts);

void peci_init(void);

#endif // _EC_PECI_H
