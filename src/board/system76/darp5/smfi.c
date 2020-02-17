#include <stdint.h>
#include <stdio.h>
#include <string.h>

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

static volatile uint8_t __xdata __at(0xC00) smfi_cmd[256];
static volatile uint8_t __xdata __at(0xD00) smfi_dbg[256];

enum SmfiCmd {
    SMFI_CMD_NONE = 0,
    SMFI_CMD_PROBE = 1,
    SMFI_CMD_BOARD = 2,
    SMFI_CMD_VERSION = 3,
    //TODO
};

enum SmfiRes {
    SMFI_RES_OK = 0,
    SMFI_RES_ERR = 1,
    //TODO
};

void smfi_init(void) {
    int i;

    // Clear command region
    for (i = 0; i < ARRAY_SIZE(smfi_cmd); i++) {
        smfi_cmd[i] = 0x00;
    }

    // Clear debug region
    for (i = 0; i < ARRAY_SIZE(smfi_dbg); i++) {
        smfi_dbg[i] = 0x00;
    }


    // H2RAM window 0 address 0xC00 - 0xCFF, read/write
    HRAMW0BA = 0xC0;
    HRAMW0AAS = 0x04;

    // H2RAM window 1 address 0xD00 - 0xDFF, read/write
    HRAMW1BA = 0xD0;
    HRAMW1AAS = 0x04;

    // Enable H2RAM window 0 and 1 using LPC I/O
    HRAMWC |= 0x13;
}

void smfi_event(void) {
    if (smfi_cmd[0]) {
        // Default to success
        smfi_cmd[1] = SMFI_RES_OK;

        switch (smfi_cmd[0]) {
            case SMFI_CMD_PROBE:
                // Signature
                smfi_cmd[2] = 0x76;
                smfi_cmd[3] = 0xEC;
                // Version
                smfi_cmd[4] = 0x01;
                break;
            case SMFI_CMD_BOARD:
                strncpy(&smfi_cmd[2], board(), ARRAY_SIZE(smfi_cmd) - 2);
                break;
            case SMFI_CMD_VERSION:
                strncpy(&smfi_cmd[2], version(), ARRAY_SIZE(smfi_cmd) - 2);
                break;
        }

        // Mark command as finished
        smfi_cmd[0] = SMFI_CMD_NONE;
    }

    if (smfi_dbg[0]) {
        int i;
        for(i = 1; (i <= (int)smfi_dbg[0]) && (i < ARRAY_SIZE(smfi_dbg)); i++) {
            putchar(smfi_dbg[i]);
        }

        // Mark debug transaction as complete
        smfi_dbg[0] = 0;
    }
}
