#ifndef _EC_SMBUS_H
#define _EC_SMBUS_H

#include <stdint.h>

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
// Receive slave address 2 for channel A
volatile uint8_t __xdata __at(0x1C3F) RESLADR2A;
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
// SMCLK timing setting for channel A
volatile uint8_t __xdata __at(0x1C40) SCLKTSA;

// Timing registers
volatile uint8_t __xdata __at(0x1C22) SMB4P7USL;
volatile uint8_t __xdata __at(0x1C23) SMB4P0USL;
volatile uint8_t __xdata __at(0x1C24) SMB300NS;
volatile uint8_t __xdata __at(0x1C25) SMB250NS;
volatile uint8_t __xdata __at(0x1C26) SMB25MS;
volatile uint8_t __xdata __at(0x1C27) SMB45P3USL;
volatile uint8_t __xdata __at(0x1C28) SMB45P3USH;

#endif // _EC_SMBUS_H
