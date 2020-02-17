#include <stdint.h>

#include <common/macro.h>

volatile uint8_t __xdata __at(0x105A) HRAMWC;
volatile uint8_t __xdata __at(0x105B) HRAMW0BA;
volatile uint8_t __xdata __at(0x105C) HRAMW1BA;
volatile uint8_t __xdata __at(0x105D) HRAMW0AAS;
volatile uint8_t __xdata __at(0x105E) HRAMW1AAS;

volatile uint8_t __xdata __at(0xC00) smfi_cmd[256];
volatile uint8_t __xdata __at(0xD00) smfi_dbg[256];

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

    // Signature
    smfi_cmd[1] = 0x76;
    smfi_cmd[2] = 0xEC;
    // Version
    smfi_cmd[3] = 0x01;

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
    //TODO: when host semaphore occurs, read command
}
