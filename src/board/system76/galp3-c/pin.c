#include "include/pin.h"

bool pin_get(struct Pin * pin) {
    if (*(pin->data) & pin->value) {
        return true;
    } else {
        return false;
    }
}

void pin_set(struct Pin * pin, bool value) {
    if (value) {
        *(pin->data) |= pin->value;
    } else {
        *(pin->data) &= ~(pin->value);
    }
}
