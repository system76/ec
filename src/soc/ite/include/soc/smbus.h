// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2019 System76, Inc.

#ifndef _SOC_SMBUS_H
#define _SOC_SMBUS_H

#include <common/macro.h>

#include <stdint.h>

#define HOSTA_BYTE_DONE BIT(7)
#define HOSTA_TIMEOUT BIT(6)
#define HOSTA_NACK BIT(5)
#define HOSTA_FAIL BIT(4)
#define HOSTA_BUS_ERR BIT(3)
#define HOSTA_DEV_ERR BIT(2)
#define HOSTA_FINISH BIT(1)
#define HOSTA_BUSY BIT(0)
#define HOSTA_ERR (HOSTA_TIMEOUT | HOSTA_NACK | HOSTA_FAIL | HOSTA_BUS_ERR | HOSTA_DEV_ERR)

// Host status for channel A
volatile uint8_t __xdata __at(0x1C00) HOSTAA;
// Host control for channel A
volatile uint8_t __xdata __at(0x1C01) HOCTLA;
// Host command for channel A
volatile uint8_t __xdata __at(0x1C02) HOCMDA;
// Transmit slave address for channel A
volatile uint8_t __xdata __at(0x1C03) TRASLAA;
// Host data 0 for channel A
volatile uint8_t __xdata __at(0x1C04) D0REGA;
// Host data 1 for channel A
volatile uint8_t __xdata __at(0x1C05) D1REGA;
// Host block data byte for channel A
volatile uint8_t __xdata __at(0x1C06) HOBDBA;
// Packet error check for channel A
volatile uint8_t __xdata __at(0x1C07) PECERCA;
// Receive slave address for channel A
volatile uint8_t __xdata __at(0x1C08) RESLADRA;
// Slave data for channel A
volatile uint8_t __xdata __at(0x1C09) SLDAA;
// SMBus pin control for channel A
volatile uint8_t __xdata __at(0x1C0A) SMBPCTLA;
// Slave status for channel A
volatile uint8_t __xdata __at(0x1C0B) SLSTAA;
// Slave interrupt control for channel A
volatile uint8_t __xdata __at(0x1C0C) SICRA;
// Notify device address for channel A
volatile uint8_t __xdata __at(0x1C0D) NDADRA;
// Notify data low byte for channel A
volatile uint8_t __xdata __at(0x1C0E) NDLBA;
// Notify data high byte for channel A
volatile uint8_t __xdata __at(0x1C0F) NDHBA;
// Host control 2 for channel A
volatile uint8_t __xdata __at(0x1C10) HOCTL2A;
// Receive slave address 2 for channel A
volatile uint8_t __xdata __at(0x1C3F) RESLADR2A;
// SMCLK timing setting for channel A
volatile uint8_t __xdata __at(0x1C40) SCLKTSA;

// Host status for channel B
volatile uint8_t __xdata __at(0x1C11) HOSTAB;
// Host control for channel B
volatile uint8_t __xdata __at(0x1C12) HOCTLB;
// Host command for channel B
volatile uint8_t __xdata __at(0x1C13) HOCMDB;
// Transmit slave address for channel B
volatile uint8_t __xdata __at(0x1C14) TRASLAB;
// Host data 0 for channel B
volatile uint8_t __xdata __at(0x1C15) D0REGB;
// Host data 1 for channel B
volatile uint8_t __xdata __at(0x1C16) D1REGB;
// Host block data byte for channel B
volatile uint8_t __xdata __at(0x1C17) HOBDBB;
// Packet error check for channel B
volatile uint8_t __xdata __at(0x1C18) PECERCB;
// Receive slave address for channel B
volatile uint8_t __xdata __at(0x1C19) RESLADRB;
// Slave data for channel B
volatile uint8_t __xdata __at(0x1C1A) SLDAB;
// SMBus pin control for channel B
volatile uint8_t __xdata __at(0x1C1B) SMBPCTLB;
// Slave status for channel B
volatile uint8_t __xdata __at(0x1C1C) SLSTAB;
// Slave interrupt control for channel B
volatile uint8_t __xdata __at(0x1C1D) SICRB;
// Notify device address for channel B
volatile uint8_t __xdata __at(0x1C1E) NDADRB;
// Notify data low byte for channel A
volatile uint8_t __xdata __at(0x1C1F) NDLBB;
// Notify data high byte for channel B
volatile uint8_t __xdata __at(0x1C20) NDHBB;
// Host control 2 for channel B
volatile uint8_t __xdata __at(0x1C21) HOCTL2B;
// Receive slave address 2 for channel B
volatile uint8_t __xdata __at(0x1C44) RESLADR2B;
// SMCLK timing setting for channel B
volatile uint8_t __xdata __at(0x1C41) SCLKTSB;

#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
// Host status for channel E
volatile uint8_t __xdata __at(0x1CA0) HOSTAE;
// Host control for channel E
volatile uint8_t __xdata __at(0x1CA1) HOCTLE;
// Host command for channel E
volatile uint8_t __xdata __at(0x1CA2) HOCMDE;
// Transmit slave address for channel E
volatile uint8_t __xdata __at(0x1CA3) TRASLAE;
// Host data 0 for channel E
volatile uint8_t __xdata __at(0x1CA4) D0REGE;
// Host data 1 for channel E
volatile uint8_t __xdata __at(0x1CA6) D1REGE;
// Host block data byte for channel E
volatile uint8_t __xdata __at(0x1CA7) HOBDBE;
// Packet error check for channel E
volatile uint8_t __xdata __at(0x1CA8) PECERCE;
// SMBus pin control for channel E
volatile uint8_t __xdata __at(0x1CA9) SMBPCTLE;
// Host control 2 for channel E
volatile uint8_t __xdata __at(0x1CAA) HOCTL2E;
// SMCLK timing setting for channel E
volatile uint8_t __xdata __at(0x1CAB) SCLKTSE;
#endif

// Timing registers
volatile uint8_t __xdata __at(0x1C22) SMB4P7USL;
volatile uint8_t __xdata __at(0x1C23) SMB4P0USL;
volatile uint8_t __xdata __at(0x1C24) SMB300NS;
volatile uint8_t __xdata __at(0x1C25) SMB250NS;
volatile uint8_t __xdata __at(0x1C26) SMB25MS;
volatile uint8_t __xdata __at(0x1C27) SMB45P3USL;
volatile uint8_t __xdata __at(0x1C28) SMB45P3USH;

#endif // _SOC_SMBUS_H
