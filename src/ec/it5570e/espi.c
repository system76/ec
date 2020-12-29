#include <common/debug.h>
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
struct VirtualWire __code VW_WAKE_N = VIRTUAL_WIRE(4, 2);
struct VirtualWire __code VW_PME_N = VIRTUAL_WIRE(4, 3);
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
struct VirtualWire __code VW_HOST_RST_WARN = VIRTUAL_WIRE(7, 0);
// Index 40 - EC to AP (platform specific)
struct VirtualWire __code VW_SUS_ACK_N = VIRTUAL_WIRE(40, 0);
// Index 41 - AP to EC (platform specific)
struct VirtualWire __code VW_SUS_WARN_N = VIRTUAL_WIRE(41, 0);
struct VirtualWire __code VW_SUS_PWRDN_ACK = VIRTUAL_WIRE(41, 1);

enum VirtualWireState vw_get(struct VirtualWire * vw) __critical {
    uint8_t index = *vw->index;
    switch ((index >> vw->shift) & VWS_HIGH) {
        case VWS_LOW:
            return VWS_LOW;
        case VWS_HIGH:
            return VWS_HIGH;
        default:
            return VWS_INVALID;
    }
}

void vw_set(struct VirtualWire * vw, enum VirtualWireState state) __critical {
    uint8_t index = *vw->index;
    index &= ~(VWS_HIGH << vw->shift);
    switch (state) {
        case VWS_LOW:
            index |= VWS_LOW << vw->shift;
            break;
        case VWS_HIGH:
            index |= VWS_HIGH << vw->shift;
            break;
        default:
            break;
    }
    *vw->index = index;
}
