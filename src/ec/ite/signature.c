// SPDX-License-Identifier: GPL-3.0-only

// ITE SoCs have a hardware mechanism to check if e-flash is programmed by a
// 16-byte signature check at power-on. It must be at a 16-byte aligned address
// at 0x40-0xF0.

#include <ec/espi.h>

// NOTE: SDCC 4.5.0 causes a conflict with the default address of 0x40, so the
// signature is moved to 0x80, which matches the address used by RISC-V SoCs
// in Zephyr.
#define EFLASH_SIG_ADDR 0x80

// Byte 7: Undocumented
// - Bit 0: Host bus type (0=eSPI, 1=LPC)
#if CONFIG_BUS_ESPI
#define HBUS 0xA4
#else
#define HBUS 0xA5
#endif

// Byte 8: "flag" (mostly undocumented)
// - Bit 6: Suspend internal-to-external clock switching (0=Disable)
// - Bit 4: 32 kHz clock selection (1=Use internal clock generator)
#define FLAG 0x95

// Byte 14: "Undocumented"; Using the documented value (0xAA) results in
// firmware not running at power-on. Proprietary firmware programs the byte
// with values such as 0x7F and 0xFF.

static __code const uint8_t __at(EFLASH_SIG_ADDR) SIGNATURE[16] = {
    0xA5, 0xA5, 0xA5, 0xA5, 0xA5, 0xA5, HBUS, FLAG,
    0x85, 0x12, 0x5A, 0x5A, 0xAA, 0x00, 0x55, 0x55,
};
