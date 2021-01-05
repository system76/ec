// SPDX-License-Identifier: GPL-3.0-only
//
// This defines a protocol for clients on the AP (application processor) to
// communicate with the EC. The protocol is polled, and uses semaphores to
// ensure correct sequencing.
//
// The client should check that the SMFI_CMD_CMD register is set to CMD_NONE,
// indicating that the EC is waiting for a command. The client should set the
// SMFI_CMD_DATA region as necessary for the command they wish to issue. They
// should then set the SMFI_CMD_CMD byte to indicate to the EC what command to
// execute. The EC will execute the command, and then set SMFI_CMD_RES to the
// correct result. It will finally set SMFI_CMD_CMD to CMD_NONE, to indicate
// the command is complete and the result is available. The client should only
// read the SMFI_CMD_RES value when SMFI_CMD_CMD is set to CMD_NONE.

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef __SCRATCH__
    #include <board/scratch.h>
    #include <board/kbled.h>
    #include <board/keymap.h>
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
// Host RAM window 2 base address
volatile uint8_t __xdata __at(0x1076) HRAMW2BA;
// Host RAM window 3 base address
volatile uint8_t __xdata __at(0x1077) HRAMW3BA;
// Host RAM window 2 access allow size
volatile uint8_t __xdata __at(0x1078) HRAMW2AAS;
// Host RAM window 3 access allow size
volatile uint8_t __xdata __at(0x1079) HRAMW3AAS;

// EC indirect flash access
volatile uint8_t __xdata __at(0x103B) ECINDAR0;
volatile uint8_t __xdata __at(0x103C) ECINDAR1;
volatile uint8_t __xdata __at(0x103D) ECINDAR2;
volatile uint8_t __xdata __at(0x103E) ECINDAR3;
volatile uint8_t __xdata __at(0x103F) ECINDDR;

// Command region - allows client to send commands to EC
#define SMFI_CMD_CMD 0x00
#define SMFI_CMD_RES 0x01
#define SMFI_CMD_DATA 0x02
static volatile uint8_t __xdata __at(0xE00) smfi_cmd[256];

// Debug region - ring buffer of EC firmware prints
#define SMFI_DBG_TAIL 0x00
static volatile uint8_t __xdata __at(0xF00) smfi_dbg[256];

#if !defined(__SCRATCH__)
void smfi_init(void) {
    int i;

    // Clear command region
    for (i = (SMFI_CMD_CMD + 1); i < ARRAY_SIZE(smfi_cmd); i++) {
        smfi_cmd[i] = 0x00;
    }
    // Clear host command last
    smfi_cmd[SMFI_CMD_CMD] = 0x00;

    // Clear debug region
    for (i = (SMFI_DBG_TAIL + 1); i < ARRAY_SIZE(smfi_dbg); i++) {
        smfi_dbg[i] = 0x00;
    }
    // Clear tail last
    smfi_dbg[SMFI_DBG_TAIL] = 0x00;

    // H2RAM window 0 address 0xE00 - 0xEFF, read/write
    HRAMW0BA = 0xE0;
    HRAMW0AAS = 0x04;

    // H2RAM window 1 address 0xF00 - 0xFFF, read-only
    HRAMW1BA = 0xF0;
    HRAMW1AAS = 0x34;

    // Enable H2RAM window 0 and 1 using LPC I/O
    HRAMWC |= (1 << 4) | (1 << 1) | (1 << 0);

    // Enable backup ROM access
    FLHCTRL3 |= (1 << 3);
}

static enum Result cmd_print(void) {
    uint8_t flags = smfi_cmd[SMFI_CMD_DATA];
    uint8_t len = smfi_cmd[SMFI_CMD_DATA + 1];

    uint8_t i;
    for (i = 0; (i < len) && ((i + SMFI_CMD_DATA + 2) < ARRAY_SIZE(smfi_cmd)); i++) {
        putchar(smfi_cmd[i + SMFI_CMD_DATA + 2]);
    }

    smfi_cmd[SMFI_CMD_DATA + 1] = i;

    return RES_OK;
}

static enum Result cmd_fan_get(void) {
    switch (smfi_cmd[SMFI_CMD_DATA]) {
        case 0:
            // Get duty of fan 0
            smfi_cmd[SMFI_CMD_DATA + 1] = DCR2;
            return RES_OK;
        case 1:
            // Get duty of fan 1
            //TODO: only allow on platforms like addw2
            smfi_cmd[SMFI_CMD_DATA + 1] = DCR4;
            return RES_OK;
    }

    // Failed if fan not found
    return RES_ERR;
}

static enum Result cmd_fan_set(void) {
    switch (smfi_cmd[SMFI_CMD_DATA]) {
        case 0:
            // Set duty cycle of fan 0
            DCR2 = smfi_cmd[SMFI_CMD_DATA + 1];
            return RES_OK;
        case 1:
            // Set duty cycle of fan 1
            //TODO: only allow on platforms like addw2
            DCR4 = smfi_cmd[SMFI_CMD_DATA + 1];
            return RES_OK;
    }

    // Failed if fan not found
    return RES_ERR;
}

static enum Result cmd_keymap_get(void) {
    int layer = smfi_cmd[SMFI_CMD_DATA];
    int output = smfi_cmd[SMFI_CMD_DATA + 1];
    int input = smfi_cmd[SMFI_CMD_DATA + 2];
    uint16_t key = 0;
    if (keymap_get(layer, output, input, &key)) {
        smfi_cmd[SMFI_CMD_DATA + 3] = (uint8_t)key;
        smfi_cmd[SMFI_CMD_DATA + 4] = (uint8_t)(key >> 8);
        return RES_OK;
    } else {
        return RES_ERR;
    }
}

static enum Result cmd_keymap_set(void) {
    int layer = smfi_cmd[SMFI_CMD_DATA];
    int output = smfi_cmd[SMFI_CMD_DATA + 1];
    int input = smfi_cmd[SMFI_CMD_DATA + 2];
    uint16_t key =
        ((uint16_t)smfi_cmd[SMFI_CMD_DATA + 3]) |
        (((uint16_t)smfi_cmd[SMFI_CMD_DATA + 4]) << 8);
    //TODO: consider only setting if the key has changed
    if (keymap_set(layer, output, input, key)) {
        //TODO: should we write config on every change?
        if (keymap_save_config()) {
            return RES_OK;
        } else {
            //TODO: need a different error code?
            return RES_ERR;
        }
    } else {
        return RES_ERR;
    }
}

static enum Result cmd_led_get_value(void) {
    uint8_t index = smfi_cmd[SMFI_CMD_DATA];
    if (index == CMD_LED_INDEX_ALL) {
        smfi_cmd[SMFI_CMD_DATA + 1] = kbled_get();
        smfi_cmd[SMFI_CMD_DATA + 2] = kbled_max();
        return RES_OK;
    } else {
        return RES_ERR;
    }
}

static enum Result cmd_led_set_value(void) {
    uint8_t index = smfi_cmd[SMFI_CMD_DATA];
    if (index == CMD_LED_INDEX_ALL) {
        kbled_set(smfi_cmd[SMFI_CMD_DATA + 1]);
        return RES_OK;
    } else {
        return RES_ERR;
    }
}

static enum Result cmd_led_get_color(void) {
    uint8_t index = smfi_cmd[SMFI_CMD_DATA];
    if (index == CMD_LED_INDEX_ALL) {
        uint32_t color = kbled_get_color();
        smfi_cmd[SMFI_CMD_DATA + 1] = (uint8_t)(color >> 16);
        smfi_cmd[SMFI_CMD_DATA + 2] = (uint8_t)(color >> 8);
        smfi_cmd[SMFI_CMD_DATA + 3] = (uint8_t)(color >> 0);
        return RES_OK;
    } else {
        return RES_ERR;
    }
}

static enum Result cmd_led_set_color(void) {
    uint8_t index = smfi_cmd[SMFI_CMD_DATA];
    if (index == CMD_LED_INDEX_ALL) {
        kbled_set_color(
            (((uint32_t)smfi_cmd[SMFI_CMD_DATA + 1]) << 16) |
            (((uint32_t)smfi_cmd[SMFI_CMD_DATA + 2]) << 8) |
            (((uint32_t)smfi_cmd[SMFI_CMD_DATA + 3]) << 0)
        );
        return RES_OK;
    } else {
        return RES_ERR;
    }
}
#endif // !defined(__SCRATCH__)

#if defined(__SCRATCH__)
static enum Result cmd_spi_scratch(void) __critical {
    uint8_t flags = smfi_cmd[SMFI_CMD_DATA];
    uint8_t len = smfi_cmd[SMFI_CMD_DATA + 1];

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
    for (i = 0; (i < len) && ((i + SMFI_CMD_DATA + 2) < ARRAY_SIZE(smfi_cmd)); i++) {
        if (flags & CMD_SPI_FLAG_READ) {
            smfi_cmd[i + SMFI_CMD_DATA + 2] = ECINDDR;
        } else {
            ECINDDR = smfi_cmd[i + SMFI_CMD_DATA + 2];
        }
    }

    // Set actually read/written count
    smfi_cmd[SMFI_CMD_DATA + 1] = i;

    if (flags & CMD_SPI_FLAG_DISABLE) {
        // Disable chip
        ECINDAR1 = 0xFE;
        ECINDDR = 0;
    }

    return RES_OK;
}
#endif // defined(__SCRATCH__)

static enum Result cmd_spi(void) {
#if defined(__SCRATCH__)
    return cmd_spi_scratch();
#else // defined(__SCRATCH__)
    if (smfi_cmd[SMFI_CMD_DATA] & CMD_SPI_FLAG_SCRATCH) {
        scratch_trampoline();
    }

    // Cannot use follow mode unless running from scratch rom
    return RES_ERR;
#endif // defined(__SCRATCH__)
}

static enum Result cmd_reset(void) {
    // Attempt to trigger watchdog reset
    ETWCFG |= (1 << 5);
    EWDKEYR = 0;

    // Failed if it got this far
    return RES_ERR;
}

// Set a watchdog timer of 10 seconds
void smfi_watchdog(void) {
    ET1CNTLLR = 0xFF;
    EWDCNTLLR = 0xFF;
    EWDCNTLHR = 0x04;
}

void smfi_event(void) {
    if (smfi_cmd[SMFI_CMD_CMD]) {
#if defined(__SCRATCH__)
        // If in scratch ROM, restart watchdog timer when command received
        smfi_watchdog();
#endif

        switch (smfi_cmd[SMFI_CMD_CMD]) {
#if !defined(__SCRATCH__)
            case CMD_PROBE:
                // Signature
                smfi_cmd[SMFI_CMD_DATA + 0] = 0x76;
                smfi_cmd[SMFI_CMD_DATA + 1] = 0xEC;
                // Version
                smfi_cmd[SMFI_CMD_DATA + 2] = 0x01;
                //TODO: bitmask of implemented commands?
                // Always successful
                smfi_cmd[SMFI_CMD_RES] = RES_OK;
                break;
            case CMD_BOARD:
                strncpy(&smfi_cmd[SMFI_CMD_DATA], board(), ARRAY_SIZE(smfi_cmd) - SMFI_CMD_DATA);
                // Always successful
                smfi_cmd[SMFI_CMD_RES] = RES_OK;
                break;
            case CMD_VERSION:
                strncpy(&smfi_cmd[SMFI_CMD_DATA], version(), ARRAY_SIZE(smfi_cmd) - SMFI_CMD_DATA);
                // Always successful
                smfi_cmd[SMFI_CMD_RES] = RES_OK;
                break;
            case CMD_PRINT:
                smfi_cmd[SMFI_CMD_RES] = cmd_print();
                break;
            case CMD_FAN_GET:
                smfi_cmd[SMFI_CMD_RES] = cmd_fan_get();
                break;
            case CMD_FAN_SET:
                smfi_cmd[SMFI_CMD_RES] = cmd_fan_set();
                break;
            case CMD_KEYMAP_GET:
                smfi_cmd[SMFI_CMD_RES] = cmd_keymap_get();
                break;
            case CMD_KEYMAP_SET:
                smfi_cmd[SMFI_CMD_RES] = cmd_keymap_set();
                break;
            case CMD_LED_GET_VALUE:
                smfi_cmd[SMFI_CMD_RES] = cmd_led_get_value();
                break;
            case CMD_LED_SET_VALUE:
                smfi_cmd[SMFI_CMD_RES] = cmd_led_set_value();
                break;
            case CMD_LED_GET_COLOR:
                smfi_cmd[SMFI_CMD_RES] = cmd_led_get_color();
                break;
            case CMD_LED_SET_COLOR:
                smfi_cmd[SMFI_CMD_RES] = cmd_led_set_color();
                break;
#endif // !defined(__SCRATCH__)
            case CMD_SPI:
                smfi_cmd[SMFI_CMD_RES] = cmd_spi();
                break;
            case CMD_RESET:
                smfi_cmd[SMFI_CMD_RES] = cmd_reset();
                break;
            default:
                // Command not found
                smfi_cmd[SMFI_CMD_RES] = RES_ERR;
                break;
        }

        // Mark command as finished
        smfi_cmd[SMFI_CMD_CMD] = CMD_NONE;
    }
}

void smfi_debug(unsigned char byte) {
    int tail = (int)smfi_dbg[SMFI_DBG_TAIL];
    tail++;
    if (tail >= ARRAY_SIZE(smfi_dbg)) {
        tail = SMFI_DBG_TAIL + 1;
    }
    smfi_dbg[tail] = byte;
    smfi_dbg[SMFI_DBG_TAIL] = (uint8_t)tail;
}
