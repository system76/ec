#include <stdio.h>

#include <ec/kbc.h>

struct Kbc __code KBC = {
    .control = &KBHICR,
    .irq = &KBIRQR,
    .status = &KBHISR,
    .keyboard_out = &KBHIKDOR,
    .mouse_out = &KBHIMDOR,
    .data_in = &KBHIDIR,
};

uint8_t kbc_status(struct Kbc * kbc) {
    return *(kbc->status);
}

uint8_t kbc_read(struct Kbc * kbc) {
    return *(kbc->data_in);
}

void kbc_keyboard(struct Kbc * kbc, uint8_t data) {
    *(kbc->keyboard_out) = data;
}

void kbc_mouse(struct Kbc * kbc, uint8_t data) {
    *(kbc->mouse_out) = data;
}

void kbc_event(struct Kbc * kbc) {
    uint8_t sts = kbc_status(kbc);
    if (sts & KBC_STS_IBF) {
        uint8_t data = kbc_read(kbc);
        if (sts & KBC_STS_CMD) {
            printf("kbc cmd: %02X\n", data);
        } else {
            printf("kbc data: %02X\n", data);
        }
    }
}
