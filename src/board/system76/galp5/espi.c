#include <8051.h>
#include <stdint.h>

#include <board/espi.h>
#include <common/debug.h>
#include <common/macro.h>
#include <ec/gctrl.h>
#include <ec/ecpm.h>
#include <ec/espi.h>

// Not all wires are defined or implemented
// Index 2 - AP to EC
struct VirtualWire __code VW_SLP_S3_N = VIRTUAL_WIRE(2, 0);
struct VirtualWire __code VW_SLP_S4_N = VIRTUAL_WIRE(2, 1);
struct VirtualWire __code VW_SLP_S5_N = VIRTUAL_WIRE(2, 2);
// Index 3 - AP to EC
struct VirtualWire __code VW_SUS_STAT_N = VIRTUAL_WIRE(3, 0);
struct VirtualWire __code VW_PLTRST_N = VIRTUAL_WIRE(3, 1);
struct VirtualWire __code VW_OOB_RST_WARN = VIRTUAL_WIRE(3, 2);
// Index 4 - EC to AP
struct VirtualWire __code VW_OOB_RST_ACK = VIRTUAL_WIRE(4, 0);
// Index 5 - EC to AP
struct VirtualWire __code VW_BOOT_LOAD_DONE = VIRTUAL_WIRE(5, 0);
struct VirtualWire __code VW_ERROR_FATAL = VIRTUAL_WIRE(5, 1);
struct VirtualWire __code VW_ERROR_NONFATAL = VIRTUAL_WIRE(5, 2);
struct VirtualWire __code VW_BOOT_LOAD_STATUS = VIRTUAL_WIRE(5, 3);
// Index 6 - EC to AP
struct VirtualWire __code VW_SCI_N = VIRTUAL_WIRE(6, 0);
struct VirtualWire __code VW_SMI_N = VIRTUAL_WIRE(6, 1);
struct VirtualWire __code VW_RCIN_N = VIRTUAL_WIRE(6, 2);
struct VirtualWire __code VW_HOST_RST_ACK = VIRTUAL_WIRE(6, 3);
// Index 7 - AP to EC

// Index 40 - EC to AP (platform specific)
struct VirtualWire __code VW_SUS_ACK_N = VIRTUAL_WIRE(40, 1);
// Index 41 - AP to EC (platform specific)
struct VirtualWire __code VW_SUS_WARN_N = VIRTUAL_WIRE(41, 1);
struct VirtualWire __code VW_SUS_PWRDN_ACK = VIRTUAL_WIRE(41, 2);

enum VirtualWireState {
    VWS_INVALID = 0x00,
    VWS_LOW = 0x10,
    VWS_HIGH = 0x11,
};

enum VirtualWireState vw_get(struct VirtualWire * vw) __critical {
    switch (((*vw->index) >> vw->shift) & VWS_HIGH) {
        case VWS_LOW:
            return VWS_LOW;
        case VWS_HIGH:
            return VWS_HIGH;
        default:
            return VWS_INVALID;
    }
}

void vw_set(struct VirtualWire * vw, enum VirtualWireState state) __critical {
    switch (state) {
        case VWS_LOW:
            *vw->index &= ~(1 << vw->shift);
            *vw->index |= (VWS_LOW << vw->shift);
            return;
        case VWS_HIGH:
            *vw->index |= (VWS_HIGH << vw->shift);
            return;
        default:
            *vw->index &= ~(VWS_HIGH << vw->shift);
            return;
    }
}

void espi_init(void) {
    DEBUG("SPCTRL3 is 0x%02X\n", SPCTRL3);

    // Port 80h debug
    SPCTRL3 |= BIT(5);

    DEBUG("SPCTRL3 is now 0x%02X\n", SPCTRL3);

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

    value = ESGCTRL1;
    if (value != BIT(7)) {
        DEBUG("ESGCTRL1 %X", value);
        ESGCTRL1 = BIT(7);
        DEBUG(" = %X\n", ESGCTRL1);
    }

    value = ESGCTRL2;
    if (value != BIT(4)) {
        DEBUG("ESGCTRL2 %X", value);
        ESGCTRL2 = BIT(4);
        DEBUG(" = %X\n", ESGCTRL2);
    }

    value = VWCTRL0;
    if (value != BIT(7)) {
        DEBUG("VWCTRL0 %X", value);
        VWCTRL0 = BIT(7);
        DEBUG(" = %X\n", VWCTRL0);
    }
}

void espi_event(void) {
    uint8_t value;

    // Detect PUT_PC
    value = ESPCTRL0;
    if (value & BIT(7)) {
        DEBUG("ESPCTRL0 %X\n", value);
        ESPCTRL0 = BIT(7);
    }

    // Detect channel enabled
    value = ESGCTRL0;
    if (value) {
        DEBUG("ESGCTRL0 %X\n", value);
        ESGCTRL0 = value;
    }

    // Detect a change in channel 0
    static uint8_t last_esc0cac0 = 0;
    value = ESC0CAC0;
    if (value != last_esc0cac0) {
        DEBUG("ESC0CAC0 %X\n", value);
        last_esc0cac0 = value;
    }

    // Detect a change in channel 1
    static uint8_t last_esc1cac0 = 0;
    value = ESC1CAC0;
    if (value != last_esc1cac0) {
        DEBUG("ESC1CAC0 %X\n", value);
        if ((value & BIT(0)) && !(last_esc1cac0 & BIT(0))) {
            DEBUG("eSPI enable VW channel\n");
            vw_set(&VW_SUS_ACK_N, VWS_LOW);
        }
        last_esc1cac0 = value;
    }

    // Detect a change in channel 2
    static uint8_t last_esc2cac0 = 0;
    value = ESC2CAC0;
    if (value != last_esc2cac0) {
        DEBUG("ESC2CAC0 %X\n", value);
        if ((value & BIT(0)) && !(last_esc2cac0 & BIT(0))) {
            DEBUG("eSPI enable OOB channel\n");
            vw_set(&VW_OOB_RST_ACK, VWS_LOW);
        }
        last_esc2cac0 = value;
    }

    // Detect a change in channel 3
    static uint8_t last_esc3cac0 = 0;
    value = ESC3CAC0;
    if (value != last_esc3cac0) {
        DEBUG("ESC3CAC0 %X\n", value);
        if ((value & BIT(0)) && !(last_esc3cac0 & BIT(0))) {
            DEBUG("eSPI enable flash channel\n");
            vw_set(&VW_BOOT_LOAD_STATUS, VWS_HIGH);
            vw_set(&VW_BOOT_LOAD_DONE, VWS_HIGH);
        }
        last_esc3cac0 = value;
    }

    // Detect updated virtual wires
    value = VWCTRL1;
    if (value) {
        DEBUG("VWCTRL1 %X\n", value);
        VWCTRL1 = value;
    }

    static enum VirtualWireState last_sus_pwrdn_ack = VWS_INVALID;
    enum VirtualWireState sus_pwrdn_ack = vw_get(&VW_SUS_PWRDN_ACK);
    if (sus_pwrdn_ack != last_sus_pwrdn_ack) {
        DEBUG("VW_SUS_PWRDN_ACK %X\n", sus_pwrdn_ack);
        last_sus_pwrdn_ack = sus_pwrdn_ack;
    }

    static enum VirtualWireState last_sus_warn_n = VWS_INVALID;
    enum VirtualWireState sus_warn_n = vw_get(&VW_SUS_WARN_N);
    if (sus_warn_n != last_sus_warn_n) {
        DEBUG("VW_SUS_WARN_N %X\n", sus_warn_n);
        vw_set(&VW_SUS_ACK_N, sus_warn_n);
        last_sus_warn_n = sus_warn_n;
    }

    static enum VirtualWireState last_pltrst_n = VWS_INVALID;
    enum VirtualWireState pltrst_n = vw_get(&VW_PLTRST_N);
    if (pltrst_n != last_pltrst_n) {
        DEBUG("VW_PLTRST_N %X\n", pltrst_n);
        if (pltrst_n == VWS_HIGH) {
            DEBUG("eSPI host startup\n");
            vw_set(&VW_SCI_N, VWS_HIGH);
            vw_set(&VW_SMI_N, VWS_HIGH);
            vw_set(&VW_RCIN_N, VWS_HIGH);
            vw_set(&VW_HOST_RST_ACK, VWS_HIGH);
        }
        last_pltrst_n = pltrst_n;
    }
}
