// SPDX-License-Identifier: GPL-3.0-only

#include <arch/delay.h>
#include <arch/time.h>
#include <board/parallel.h>
#include <ec/kbscan.h>

#include <stdbool.h>

/*
 * nWRITE   = KSI[0]
 * nDATASTB = KSI[1]
 * nRESET   = KSI[2]
 * nADDRSTB = KSI[3]
 *
 * AD[8:1]  = KSOL[7:0]
 * nINTR    = KSOH[0]
 * nWAIT    = KSOH[1]
 */

#define CTL_WRITE   (1 << 0)
#define CTL_DATA    (1 << 1)
#define CTL_RESET   (1 << 2)
#define CTL_ADDR    (1 << 3)

#define STS_WAIT    (1 << 1)

// Maximum peripheral response time in ms
#define PARALLEL_TIMEOUT 10

bool parallel_debug = false;

static bool parallel_wait_peripheral(uint8_t mask, uint8_t value) {
    uint32_t start = time_get();

    while (time_get() < start + PARALLEL_TIMEOUT) {
        if ((KSOHGDMRR & mask) == value) {
            return true;
        }
    }

    return false;
}

bool parallel_init(void) {
    // XXX: Needed? Pull-ups, open-drain are always disabled in GPIO mode
    KSOCTRL = 0;
    // XXX: Needed? OVRPPK is for KBS mode, pull-ups are always disabled in GPIO mode
    KSICTRLR = 0;

    // Set all outputs to GPIO mode, low, and inputs
    KSOL = 0;
    KSOLGCTRL = 0xFF;
    KSOLGOEN = 0;
    KSOH1 = 0;
    KSOHGCTRL = 0xFF;
    KSOHGOEN = 0;
    KSOH2 = 0;

    // Set control lines as outputs, low
    KSIGCTRL = 0xFF;
    KSIGOEN = 0x0F;
    KSIGDAT = 0;

    // Assert nRESET
    KSIGDAT &= ~CTL_RESET;

    // Deassert nWRITE, nDATASTB, nADDRSTB
    KSIGDAT |= CTL_WRITE | CTL_DATA | CTL_ADDR;

    // PUll up nWAIT
    KSOH1 |= STS_WAIT;

    // Pull up data lines
    KSOL = 0xFF;

    // Deassert nRESET
    KSIGDAT |= CTL_RESET;

    // Check if there is a peripheral waiting
    return parallel_wait_peripheral(STS_WAIT, 0);
}

int parallel_write(uint8_t * data, int length) {
    // Assert nWRITE
    KSIGDAT &= ~CTL_WRITE;

    // Set data lines as outputs
    KSOLGOEN = 0xFF;

    int i;
    for (i = 0; i < length; i++) {
        // Wait for peripheral to indicate it's ready for next cycle
        if (!parallel_wait_peripheral(STS_WAIT, 0)) {
            break;
        }

        // Write data to port
        KSOL = data[i];

        // Assert nDATASTB
        KSIGDAT &= ~CTL_DATA;

        // Wait for peripheral to indicate it's processing
        if (!parallel_wait_peripheral(STS_WAIT, STS_WAIT)) {
            KSIGDAT |= CTL_DATA;
            break;
        }

        // Deassert nDATASTB
        KSIGDAT |= CTL_DATA;

        // Wait for peripheral to indicate it's ready for next cycle
        if (!parallel_wait_peripheral(STS_WAIT, 0)) {
            break;
        }

        // Reset data lines to high
        KSOL = 0xFF;
    }

    // Set data lines back to inputs
    KSOLGOEN = 0;

    // Deassert nWRITE
    KSIGDAT |= CTL_WRITE;

    return i;
}
