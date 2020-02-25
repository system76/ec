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

volatile uint8_t __xdata __at(0x103B) ECINDAR0;
volatile uint8_t __xdata __at(0x103C) ECINDAR1;
volatile uint8_t __xdata __at(0x103D) ECINDAR2;
volatile uint8_t __xdata __at(0x103E) ECINDAR3;
volatile uint8_t __xdata __at(0x103F) ECINDDR;

volatile uint8_t __xdata __at(0x1F01) ETWCFG;
volatile uint8_t __xdata __at(0x1F07) EWDKEYR;

static volatile uint8_t __xdata __at(0xC00) smfi_cmd[256];
static volatile uint8_t __xdata __at(0xD00) smfi_dbg[256];

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


    // H2RAM window 0 address 0xC00 - 0xCFF, read/write
    HRAMW0BA = 0xC0;
    HRAMW0AAS = 0x04;

    // H2RAM window 1 address 0xD00 - 0xDFF, read/write
    HRAMW1BA = 0xD0;
    HRAMW1AAS = 0x04;

    // Enable H2RAM window 0 and 1 using LPC I/O
    HRAMWC |= 0x13;
}

static enum Result cmd_debug(void) {
    int i;
    for (i = 2; i < ARRAY_SIZE(smfi_cmd); i++) {
        uint8_t b = smfi_cmd[i];
        if (b == 0) break;
        putchar(b);
    }

    return RES_OK;
}

static enum Result cmd_spi(void) {
    uint8_t flags = smfi_cmd[2];

#ifdef __SCRATCH__
    int len = (int)smfi_cmd[3];

    // Enable chip (internal)
    ECINDAR3 = 0x7F;
    ECINDAR2 = 0xFF;
    ECINDAR1 = 0xFD;
    ECINDAR0 = 0x00;

    // Read or write len bytes
    int i;
    for (i = 0; i < len && (i + 4) < ARRAY_SIZE(smfi_cmd); i++) {
        if (flags & CMD_SPI_FLAG_READ) {
            smfi_cmd[i + 4] = ECINDDR;
        } else {
            ECINDDR = smfi_cmd[i + 4];
        }
    }

    // Set actually read/written count
    smfi_cmd[3] = (uint8_t)i;

    if (flags & CMD_SPI_FLAG_DISABLE) {
        // Disable chip
        ECINDAR1 = 0xFE;
        ECINDDR = 0;
    }

    return RES_OK;
#else
    if (flags & CMD_SPI_FLAG_SCRATCH) {
        scratch_trampoline();
    }

    // Cannot use SPI bus while running EC from SPI, or trampoline failed
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

void smfi_event(void) {
    if (smfi_cmd[0]) {
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
            case CMD_DEBUG:
                smfi_cmd[1] = cmd_debug();
                break;
            case CMD_SPI:
                smfi_cmd[1] = cmd_spi();
                break;
            case CMD_RESET:
                smfi_cmd[1] = cmd_reset();
                break;
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
