#include <8051.h>
#include <stdint.h>

#include <board/espi.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/ecpm.h>
#include <ec/gctrl.h>

void espi_init(void) {
    DEBUG("SPCTRL3 %X", SPCTRL3);

    // eSPI port 80h debug
    SPCTRL3 |= BIT(5);

    DEBUG(" = %X\n", SPCTRL3);

    espi_reset();
}

void espi_reset(void) {
    uint8_t value;

    value = ESGCAC2;
    if ((value & 0b111) != 0b011) {
        DEBUG("ESGCAC2 %X", value);
        ESGCAC2 = (value & ~0b111) | 0b011;
        DEBUG(" = %X\n", ESGCAC2);
    }
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
            vw_set(&VW_SUS_ACK_N, VWS_LOW);
        }
        if (value & BIT(2)) {
            DEBUG("  OOB enabled\n");
            vw_set(&VW_OOB_RST_ACK, VWS_LOW);
        }
        if (value & BIT(3)) {
            DEBUG("  Flash enabled\n");
            // Custom code to set VW_BOOT_LOAD* simultaneously
            VWIDX5 = 0x99;
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
}
