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
// Index 4 - EC to AP
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
    DEBUG("RSTS is 0x%02X\n", RSTS);

    // Disable reset of PNPCFG with LPCPD# and LPCRST#
    RSTS &= ~BIT(3);

    DEBUG("RSTS is now 0x%02X\n", RSTS);

    DEBUG("SPCTRL3 is 0x%02X\n", SPCTRL3);

    // Port 80h debug
    SPCTRL3 |= BIT(5);

    DEBUG("SPCTRL3 is now 0x%02X\n", SPCTRL3);

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

    espi_reset();
}

void espi_reset(void) {
    uint8_t value;

    value = ESGCAC2;
    if ((value & 0b111) != 0b011) {
        DEBUG("ESGCAC2 %X", value);
        ESGCAC2 &= ~0b111;
        ESGCAC2 |= 0b011;
        DEBUG(" = %X\n", ESGCAC2);
    }

    value = ESC3CAC2;
    if (value) {
        DEBUG("ESC3CAC2 %X", value);
        ESC3CAC2 = 0;
        DEBUG(" = %X\n", ESC3CAC2);
    }

    value = ESGCTRL0;
    if (value) {
        DEBUG("ESGCTRL0 %X\n", value);
        ESGCTRL0 = value;
    }

    value = ESGCTRL1;
    if (!(value & BIT(7))) {
        DEBUG("ESGCTRL1 %X", value);
        ESGCTRL1 |= BIT(7);
        DEBUG(" = %X\n", ESGCTRL1);
    }

    value = ESGCTRL2;
    if (value) {
        DEBUG("ESGCTRL2 %X", value);
        ESGCTRL2 = 0;
        DEBUG(" = %X\n", ESGCTRL2);
    }

    value = ESGCTRL3;
    if (value) {
        DEBUG("ESGCTRL3 %X", value);
        ESGCTRL3 = 0;
        DEBUG(" = %X\n", ESGCTRL3);
    }

    value = ESPCTRL0;
    if (value & BIT(7)) {
        DEBUG("ESPCTRL0 %X\n", value);
        ESPCTRL0 = BIT(7);
    }

    value = VWCTRL0;
    if ((value & (BIT(0) | BIT(1) | BIT(7))) != (BIT(0) | BIT(1) | BIT(7))) {
        DEBUG("VWCTRL1 %X", value);
        VWCTRL0 |= BIT(0) | BIT(1) | BIT(7);
        DEBUG(" = %X\n", VWCTRL1);
    }

    value = VWCTRL1;
    if (value) {
        DEBUG("VWCTRL1 %X\n", value);
        VWCTRL1 = value;
    }

    value = VWCTRL2;
    if (value != BIT(6)) {
        DEBUG("VWCTRL2 %X", value);
        VWCTRL2 = BIT(6);
        DEBUG(" = %X\n", VWCTRL2);
    }

    // Automatically send boot load status and boot load done
    value = VWCTRL5;
    if (!(value & BIT(0))) {
        DEBUG("VWCTRL5 %X", value);
        VWCTRL5 |= BIT(0);
        DEBUG(" = %X\n", VWCTRL5);
    }

    // Automatically send SUS_ACK#
    value = VWCTRL5;
    if (!(value & BIT(1))) {
        DEBUG("VWCTRL5 %X", value);
        VWCTRL5 |= BIT(1);
        DEBUG(" = %X\n", VWCTRL5);
    }

    value = VWCTRL6;
    if (value) {
        DEBUG("VWCTRL6 %X\n", value);
        VWCTRL6 = value;
    }

    vw_set(&VW_SCI_N, VWS_HIGH);
    vw_set(&VW_SMI_N, VWS_HIGH);
    vw_set(&VW_RCIN_N, VWS_HIGH);
    vw_set(&VW_HOST_RST_ACK, VWS_HIGH);

    vw_set(&VW_BOOT_LOAD_STATUS, VWS_HIGH);
    vw_set(&VW_BOOT_LOAD_DONE, VWS_HIGH);
}

void espi_event(void) {
    uint8_t value;

    static uint8_t last_esgcac0 = 0;
    value = ESGCAC0;
    if (value != last_esgcac0) {
        DEBUG("ESGCAC0 %X\n", value);
        last_esgcac0 = value;
    }

    static uint8_t last_esc0cac0 = 0;
    value = ESC0CAC0;
    if (value != last_esc0cac0) {
        DEBUG("ESC0CAC0 %X\n", value);
        last_esc0cac0 = value;
    }

    static uint8_t last_esc1cac0 = 0;
    value = ESC1CAC0;
    if (value != last_esc1cac0) {
        DEBUG("ESC1CAC0 %X\n", value);
        last_esc1cac0 = value;
    }

    static uint8_t last_esc2cac0 = 0;
    value = ESC2CAC0;
    if (value != last_esc2cac0) {
        DEBUG("ESC2CAC0 %X\n", value);
        last_esc2cac0 = value;
    }

    static uint8_t last_esc3cac0 = 0;
    value = ESC3CAC0;
    if (value != last_esc3cac0) {
        DEBUG("ESC3CAC0 %X\n", value);
        last_esc3cac0 = value;
    }

    static enum VirtualWireState last_sus_pwrdn_ack = VWS_INVALID;
    enum VirtualWireState sus_pwrdn_ack = vw_get(&VW_SUS_PWRDN_ACK);
    if (sus_pwrdn_ack != last_sus_pwrdn_ack) {
        DEBUG("VW_SUS_PWRDN_ACK %X\n", sus_pwrdn_ack);
        last_sus_pwrdn_ack = sus_pwrdn_ack;
    }
}
