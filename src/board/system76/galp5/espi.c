#include <8051.h>
#include <stdint.h>

#include <arch/delay.h>
#include <board/espi.h>
#include <board/gpio.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/ecpm.h>
#include <ec/gctrl.h>

#define DEBUG_SET(REG, MASK, BITS) { \
    DEBUG("%s @ %p: %X", #REG, &REG, REG); \
    REG = ((REG) & ~(MASK)) | (BITS); \
    DEBUG(" set to %X\n", REG); \
}

#define DEBUG_ON(REG, BITS) \
    DEBUG_SET(REG, BITS, BITS)

#define DEBUG_OFF(REG, BITS) \
    DEBUG_SET(REG, BITS, 0)

#define DEBUG_CHANGED(REG) { \
    static uint8_t last_ ## REG = 0; \
    uint8_t new_ ## REG = REG; \
    if (new_ ## REG != last_ ## REG) { \
        DEBUG( \
            "%S @ %p: %X changed to %X\n", \
            #REG, \
            &REG, \
            last_ ## REG, \
            new_ ## REG \
        ); \
        last_ ## REG = new_ ## REG; \
    } \
}

void espi_init(void) {
/*
    // Workarounds to allow changing PLL
    {
        // ESPI_CS# set to input
        GPCRM5 = GPIO_IN;
        // Disable eSPI pads
        ESGCTRL2 |= BIT(6);
    }

    DEBUG("PLLFREQ is 0x%02X\n", PLLFREQ);

    // Set PLL frequency to 64.5 MHz for eSPI
    PLLFREQ = 0b0111;

    // Prepare to sleep
    PLLCTRL = 0x01;

    // No idea why
    __asm__("nop");

    // Set power down bit
    PCON |= BIT(1);

    // Wait for "internal bus turn-around"
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");
    __asm__("nop");

    DEBUG("PLLFREQ is now 0x%02X\n", PLLFREQ);

    // Wait for PLL to stabilize
    delay_ms(5);

    // Workarounds to allow changing PLL
    {
        // Enable eSPI pads
        ESGCTRL2 &= ~BIT(6);
        // ESPI_CS# set to alternate mode
        GPCRM5 = GPIO_ALT;
    }
*/

    // Enable port 80 debug
    DEBUG_ON(SPCTRL3, BIT(5));

    // Set maximum eSPI frequency to 50MHz
    DEBUG_SET(ESGCAC2, 0b111, 0b011);

    espi_reset();
}

void espi_reset(void) {
    //TODO
}

void espi_event(void) {
    uint8_t value;

    // Detect PUT_PC
    value = ESPCTRL0;
    if (value & BIT(7)) {
        ESPCTRL0 = BIT(7);

        DEBUG("ESPCTRL0 %X\n", value);
    }

    // Detect channel enabled
    value = ESGCTRL0;
    if (value) {
        ESGCTRL0 = value;

        DEBUG("ESGCTRL0 %X\n", value);

        if (value & BIT(0)) {
            DEBUG("  PC enabled\n");
        }
        if (value & BIT(1)) {
            DEBUG("  VW enabled\n");
        }
        if (value & BIT(2)) {
            DEBUG("  OOB enabled\n");
        }
        if (value & BIT(3)) {
            DEBUG("  Flash enabled\n");
            vw_set(&VW_BOOT_LOAD_STATUS, VWS_HIGH);
            vw_set(&VW_BOOT_LOAD_DONE, VWS_HIGH);
        }
    }

    // Detect updated virtual wires
    value = VWCTRL1;
    if (value) {
        VWCTRL1 = value;

        DEBUG("VWCTRL1 %X\n", value);

        if (value & BIT(0)) {
            DEBUG("  VW 2 %X\n", VWIDX2);
        }
        if (value & BIT(1)) {
            DEBUG("  VW 3 %X\n", VWIDX3);
        }
        if (value & BIT(2)) {
            DEBUG("  VW 7 %X\n", VWIDX7);
        }
        if (value & BIT(3)) {
            DEBUG("  VW 41 %X\n", VWIDX41);
        }
    }

    // Detect when boot load virtual wires were sent
    value = VWCTRL6;
    if (value) {
        VWCTRL6 = value;

        DEBUG("VWCTRL6 %X\n", value);
    }

    // Detect when frequency changes
    DEBUG_CHANGED(ESGCTRL2);

    // Detect when I/O mode changes
    DEBUG_CHANGED(ESGCTRL3);

    // Detect when channel 0 changes
    DEBUG_CHANGED(ESC0CAC0);

    // Detect when channel 1 changes
    DEBUG_CHANGED(ESC1CAC0);

    // Detect when virtual wire count changes
    DEBUG_CHANGED(ESC1CAC2);

    // Detect when channel 2 changes
    DEBUG_CHANGED(ESC2CAC0);

    // Detect when channel 3 changes
    DEBUG_CHANGED(ESC3CAC0);

    // Detect when AP to EC virtual wires change
    DEBUG_CHANGED(VWIDX2);
    DEBUG_CHANGED(VWIDX3);
    DEBUG_CHANGED(VWIDX7);
    DEBUG_CHANGED(VWIDX41);
}
