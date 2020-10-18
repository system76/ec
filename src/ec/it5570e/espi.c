#include <common/debug.h>
#include <ec/espi.h>

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
    DEBUG("vw_set %p: %0X", vw->index, index);
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
    DEBUG(" = %0X\n", index);
    *vw->index = index;
}
