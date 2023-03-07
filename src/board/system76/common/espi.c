// SPDX-License-Identifier: GPL-3.0-only

#include <board/espi.h>

#include <arch/delay.h>
#include <board/power.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/ecpm.h>
#include <ec/gctrl.h>
#include <ec/gpio.h>

#include <8051.h>
#include <stdint.h>

#define DEBUG_SET(REG, MASK, BITS) \
    { \
        DEBUG("%s: %X", #REG, REG); \
        REG = ((REG) & ~(MASK)) | (BITS); \
        DEBUG(" set to %X\n", REG); \
    }

#define DEBUG_ON(REG, BITS) DEBUG_SET(REG, BITS, BITS)

#define DEBUG_OFF(REG, BITS) DEBUG_SET(REG, BITS, 0)

#define DEBUG_CHANGED(REG) \
    { \
        static uint8_t last_##REG = 0; \
        uint8_t new_##REG = REG; \
        if (new_##REG != last_##REG) { \
            DEBUG("%S: %X changed to %X\n", #REG, last_##REG, new_##REG); \
            last_##REG = new_##REG; \
        } \
    }

#define VW_SET_DEBUG(W, V) \
    { \
        DEBUG("%s = %X\n", #W, V); \
        vw_set(&W, V); \
    }

bool espi_host_reset = false;

void espi_init(void) {
    if (PLLFREQ != 0b0111) {
        // Workarounds to allow changing PLL
        {
            // ESPI_CS# set to input
            GPCRM5 = GPIO_IN;
            // Disable eSPI pads
            DEBUG_ON(ESGCTRL2, BIT(6));
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
            DEBUG_OFF(ESGCTRL2, BIT(6));
            // ESPI_CS# set to alternate mode
            GPCRM5 = GPIO_ALT;
        }
    }

    // Set maximum eSPI frequency to 50MHz
    DEBUG_SET(ESGCAC2, 0b111, 0b011);

    espi_event();
}

void espi_reset(void) {
    espi_event();
}

void espi_event(void) {
    uint8_t value;
    enum VirtualWireState wire;

    // Detect channel enabled
    value = ESGCTRL0;
    if (value) {
        ESGCTRL0 = value;
        DEBUG("ESGCTRL0 %X\n", value);

        if (value & BIT(1)) {
            // Set boot load status and boot load done high, once VWs can be set
            VW_SET_DEBUG(VW_BOOT_LOAD_STATUS, VWS_HIGH);
            VW_SET_DEBUG(VW_BOOT_LOAD_DONE, VWS_HIGH);
        }
    }

    // Detect updated virtual wires
    value = VWCTRL1;
    if (value) {
        VWCTRL1 = value;
        DEBUG("VWCTRL1 %X\n", value);

        if (value & BIT(0)) {
            DEBUG("VWIDX2 %X\n", VWIDX2);
        }
        if (value & BIT(1)) {
            DEBUG("VWIDX3 %X\n", VWIDX3);

            // Set OOB_RST_ACK to OOB_RST_WARN
            wire = vw_get(&VW_OOB_RST_WARN);
            if (wire != vw_get(&VW_OOB_RST_ACK)) {
                VW_SET_DEBUG(VW_OOB_RST_ACK, wire);
            }

            static enum VirtualWireState last_pltrst_n = VWS_INVALID;
            wire = vw_get(&VW_PLTRST_N);
            if (wire != last_pltrst_n) {
                DEBUG("ESPI PLTRST# %X\n", wire);
                if (wire == VWS_HIGH) {
                    // Set SCI, SMI, and RCIN to high
                    VW_SET_DEBUG(VW_SCI_N, VWS_HIGH);
                    VW_SET_DEBUG(VW_SMI_N, VWS_HIGH);
                    VW_SET_DEBUG(VW_RCIN_N, VWS_HIGH);

                    power_cpu_reset();

                    // Host reset complete
                    espi_host_reset = false;
                }
                last_pltrst_n = wire;
            }
        }
        if (value & BIT(2)) {
            DEBUG("VWIDX7 %X\n", VWIDX7);

            // Set HOST_RST_ACK to HOST_RST_WARN
            wire = vw_get(&VW_HOST_RST_WARN);
            if (wire != vw_get(&VW_HOST_RST_ACK)) {
                if (wire == VWS_HIGH) {
                    // Host reset started
                    espi_host_reset = true;
                }

                VW_SET_DEBUG(VW_HOST_RST_ACK, wire);
            }
        }
        if (value & BIT(3)) {
            DEBUG("VWIDX41 %X\n", VWIDX41);

            // Set SUS_ACK# to SUS_WARN#
            wire = vw_get(&VW_SUS_WARN_N);
            if (wire != vw_get(&VW_SUS_ACK_N)) {
                VW_SET_DEBUG(VW_SUS_ACK_N, wire);
            }
        }
        if (value & BIT(7)) {
            DEBUG("VWIDX47 %X\n", VWIDX47);
        }
    }
}
