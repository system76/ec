#include <arch/delay.h>
#include <arch/time.h>
#include <ec/kbscan.h>
#include <ec/parallel.h>

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
#define PARPORT_TIMEOUT 35

void parport_init(void) {
    // XXX: Needed? Pull-ups, open-drain are always disabled in GPIO mode
    KSOCTRL = 0;
    // XXX: Needed? OVRPPK is for KBS mode, pull-ups are always disabled in GPIO mode
    KSICTRLR = (1 << 4);

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

    // Set nWAIT high
    KSOH1 |= STS_WAIT;

    // Pull up data lines
    KSOL = 0xFF;

    // Deassert nRESET
    KSIGDAT |= CTL_RESET;
}

bool parport_wait_peripheral(uint8_t mask, uint8_t value) {
    uint32_t start = time_get();

    while (time_get() < start + PARPORT_TIMEOUT) {
        if ((KSOHGDMRR & mask) == value) {
            return true;
        }
    }

    return false;
}

int parport_write(uint8_t * data, int length) {
    // Assert nWRITE
    KSIGDAT &= ~CTL_WRITE;

    // Set data lines as outputs
    KSOLGOEN = 0xFF;

    int i;
    for (i = 0; i < length; i++) {
        // Wait for peripheral to indicate it's ready for next cycle
        if (!parport_wait_peripheral(STS_WAIT, 0)) {
            break;
        }

        // Write data to port
        KSOL = data[i];

        // Assert nDATASTB
        KSIGDAT &= ~CTL_DATA;
        delay_us(1);

        // Wait for peripheral to indicate it's ready
        if (!parport_wait_peripheral(STS_WAIT, STS_WAIT)) {
            KSIGDAT |= CTL_DATA;
            break;
        }

        // Deassert nDATASTB
        KSIGDAT |= CTL_DATA;
        delay_us(1);

        // XXX: Arduino takes a while to read?
        delay_us(5);

        // Reset data lines to high
        KSOL = 0xFF;
    }

    // Set data lines back to inputs
    KSOLGOEN = 0;

    // Deassert nWRITE
    KSIGDAT |= CTL_WRITE;

    return i;
}
