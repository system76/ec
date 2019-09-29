#include "include/kbc.h"

__code struct Kbc KBC = {
    .control = &KBHICR,
    .irq = &KBIRQR,
    .status = &KBHISR,
    .keyboard_out = &KBHIKDOR,
    .mouse_out = &KBHIMDOR,
    .data_in = &KBHIDIR,
};

void kbc_init(void) {
    *(KBC.irq) = 0;
    *(KBC.control) = 0x48;
}

unsigned char kbc_status(struct Kbc * kbc) {
    return *(kbc->status);
}

unsigned char kbc_read(struct Kbc * kbc) {
    return *(kbc->data_in);
}

void kbc_keyboard(struct Kbc * kbc, unsigned char data) {
    *(kbc->keyboard_out) = data;
}

void kbc_mouse(struct Kbc * kbc, unsigned char data) {
    *(kbc->mouse_out) = data;
}
