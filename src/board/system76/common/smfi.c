// SPDX-License-Identifier: GPL-3.0-only

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef __SCRATCH__
    #include <board/scratch.h>
#endif
#include <board/smfi.h>
#include <common/command.h>
#include <common/macro.h>
#include <common/version.h>
#include <ec/etwd.h>
#include <ec/pwm.h>

// Shared memory host semaphore
volatile uint8_t __xdata __at(0x1022) SMHSR;
// Host RAM window control
volatile uint8_t __xdata __at(0x105A) HRAMWC;
// Host RAM window 0 base address
volatile uint8_t __xdata __at(0x105B) HRAMW0BA;
// Host RAM window 1 base address
volatile uint8_t __xdata __at(0x105C) HRAMW1BA;
// Host RAM window 0 access allow size
volatile uint8_t __xdata __at(0x105D) HRAMW0AAS;
// Host RAM window 1 access allow size
volatile uint8_t __xdata __at(0x105E) HRAMW1AAS;
// Flash control register 3
volatile uint8_t __xdata __at(0x1063) FLHCTRL3;

// EC indirect flash access
volatile uint8_t __xdata __at(0x103B) ECINDAR0;
volatile uint8_t __xdata __at(0x103C) ECINDAR1;
volatile uint8_t __xdata __at(0x103D) ECINDAR2;
volatile uint8_t __xdata __at(0x103E) ECINDAR3;
volatile uint8_t __xdata __at(0x103F) ECINDDR;

static volatile uint8_t __xdata __at(0xE00) smfi_cmd[256];
static volatile uint8_t __xdata __at(0xF00) smfi_dbg[256];

static enum Result cmd_spi_scratch(void) __critical {
    uint8_t flags = smfi_cmd[2];
    uint8_t len = smfi_cmd[3];

    // Enable chip
    if (flags & CMD_SPI_FLAG_BACKUP) {
        ECINDAR3 = 0xFF;
    } else {
        ECINDAR3 = 0x7F;
    }
    ECINDAR2 = 0xFF;
    ECINDAR1 = 0xFD;
    ECINDAR0 = 0x00;

    // Read or write len bytes
    uint8_t i;
    for (i = 0; (i < len) && ((i + 4) < ARRAY_SIZE(smfi_cmd)); i++) {
        if (flags & CMD_SPI_FLAG_READ) {
            smfi_cmd[i + 4] = ECINDDR;
        } else {
            ECINDDR = smfi_cmd[i + 4];
        }
    }

    // Set actually read/written count
    smfi_cmd[3] = i;

    if (flags & CMD_SPI_FLAG_DISABLE) {
        // Disable chip
        ECINDAR1 = 0xFE;
        ECINDDR = 0;
    }

    return RES_OK;
}

void smfi_init(void) {
    int i;

    // Clear command region
    for (i = 1; i < ARRAY_SIZE(smfi_cmd); i++) {
        smfi_cmd[i] = 0x00;
    }
    // Clear host command last
    smfi_cmd[0] = 0x00;

    // Clear debug region
    for (i = 1; i < ARRAY_SIZE(smfi_dbg); i++) {
        smfi_dbg[i] = 0x00;
    }
    // Clear tail last
    smfi_dbg[0] = 0x00;


    // H2RAM window 0 address 0xE00 - 0xEFF, read/write
    HRAMW0BA = 0xE0;
    HRAMW0AAS = 0x04;

    // H2RAM window 1 address 0xF00 - 0xFFF, read/write
    HRAMW1BA = 0xF0;
    HRAMW1AAS = 0x04;

    // Enable H2RAM window 0 and 1 using LPC I/O
    HRAMWC |= 0x13;

    // Enable backup ROM access
    FLHCTRL3 |= (1 << 3);
}

static enum Result cmd_print(void) {
    uint8_t flags = smfi_cmd[2];
    uint8_t len = smfi_cmd[3];

    uint8_t i;
    for (i = 0; (i < len) && ((i + 4) < ARRAY_SIZE(smfi_cmd)); i++) {
        putchar(smfi_cmd[i + 4]);
    }

    smfi_cmd[3] = i;

    return RES_OK;
}

static enum Result cmd_spi(void) {
#ifdef __SCRATCH__
    return cmd_spi_scratch();
#else
    if (smfi_cmd[2] & CMD_SPI_FLAG_SCRATCH) {
        scratch_trampoline();
    }

    // Cannot use follow mode unless running from scratch rom
    return RES_ERR;
#endif
}

static enum Result cmd_reset(void) {
    // Attempt to trigger watchdog reset
    ETWCFG |= (1 << 5);
    EWDKEYR = 0;

    // Failed if it got this far
    return RES_ERR;
}

#ifndef __SCRATCH__
static enum Result cmd_fan_get(void) {
    switch (smfi_cmd[2]) {
        case 0:
            // Get duty of fan 0
            smfi_cmd[3] = DCR2;
            return RES_OK;
        case 1:
            // Get duty of fan 1
            //TODO: only allow on platforms like addw2
            smfi_cmd[3] = DCR4;
            return RES_OK;
    }

    // Failed if fan not found
    return RES_ERR;
}

static enum Result cmd_fan_set(void) {
    switch (smfi_cmd[2]) {
        case 0:
            // Set duty cycle of fan 0
            DCR2 = smfi_cmd[3];
            return RES_OK;
        case 1:
            // Set duty cycle of fan 1
            //TODO: only allow on platforms like addw2
            DCR4 = smfi_cmd[3];
            return RES_OK;
    }

    // Failed if fan not found
    return RES_ERR;
}
#endif

// Set a watchdog timer of 10 seconds
void smfi_watchdog(void) {
    ET1CNTLLR = 0xFF;
    EWDCNTLLR = 0xFF;
    EWDCNTLHR = 0x04;
}

void smfi_event(void) {
    if (smfi_cmd[0]) {
#ifdef __SCRATCH__
        // If in scratch ROM, restart watchdog timer when command received
        smfi_watchdog();
#endif

        switch (smfi_cmd[0]) {
            case CMD_PROBE:
                // Signature
                smfi_cmd[2] = 0x76;
                smfi_cmd[3] = 0xEC;
                // Version
                smfi_cmd[4] = 0x01;
                // Always successful
                smfi_cmd[1] = RES_OK;
                break;
            case CMD_BOARD:
                strncpy(&smfi_cmd[2], board(), ARRAY_SIZE(smfi_cmd) - 2);
                // Always successful
                smfi_cmd[1] = RES_OK;
                break;
            case CMD_VERSION:
                strncpy(&smfi_cmd[2], version(), ARRAY_SIZE(smfi_cmd) - 2);
                // Always successful
                smfi_cmd[1] = RES_OK;
                break;
            case CMD_PRINT:
                smfi_cmd[1] = cmd_print();
                break;
            case CMD_SPI:
                smfi_cmd[1] = cmd_spi();
                break;
            case CMD_RESET:
                smfi_cmd[1] = cmd_reset();
                break;
#ifndef __SCRATCH__
            case CMD_FAN_GET:
                smfi_cmd[1] = cmd_fan_get();
                break;
            case CMD_FAN_SET:
                smfi_cmd[1] = cmd_fan_set();
                break;
#endif // __SCRATCH__
            default:
                // Command not found
                smfi_cmd[1] = RES_ERR;
                break;
        }

        // Mark command as finished
        smfi_cmd[0] = CMD_NONE;
    }
}

void smfi_debug(unsigned char byte) {
    int tail = (int)smfi_dbg[0];
    tail++;
    if (tail >= ARRAY_SIZE(smfi_dbg)) {
        tail = 1;
    }
    smfi_dbg[tail] = byte;
    smfi_dbg[0] = (uint8_t)tail;
}
