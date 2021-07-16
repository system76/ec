// SPDX-License-Identifier: GPL-3.0-only

#include <arch/delay.h>
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

static bool kbc_wait(struct Kbc * kbc, int timeout) {
    while (*(kbc->status) & KBC_STS_OBF) {
        if (timeout == 0) return false;
        timeout -= 1;
        delay_us(1);
    }
    return true;
}

bool kbc_keyboard(struct Kbc * kbc, uint8_t data, int timeout) {
    if (!kbc_wait(kbc, timeout)) return false;
    *(kbc->status) &= ~0x20;
    *(kbc->keyboard_out) = data;
    return true;
}

bool kbc_mouse(struct Kbc * kbc, uint8_t data, int timeout) {
    if (!kbc_wait(kbc, timeout)) return false;
    *(kbc->status) |= 0x20;
    *(kbc->mouse_out) = data;
    return true;
}
