// SPDX-License-Identifier: GPL-3.0-only

#include <soc/kbc.h>
#include <arch/delay.h>

struct Kbc __code KBC = {
    .control = &KBHICR,
    .irq = &KBIRQR,
    .status = &KBHISR,
    .keyboard_out = &KBHIKDOR,
    .mouse_out = &KBHIMDOR,
    .data_in = &KBHIDIR,
};

uint8_t kbc_status(const struct Kbc *const kbc) {
    return *(kbc->status);
}

uint8_t kbc_read(const struct Kbc *const kbc) {
    return *(kbc->data_in);
}

static bool kbc_wait(const struct Kbc *const kbc, uint16_t timeout) {
    while (*(kbc->status) & KBC_STS_OBF) {
        if (timeout == 0)
            return false;
        timeout -= 1;
        delay_us(1);
    }
    return true;
}

bool kbc_keyboard(struct Kbc *const kbc, uint8_t data, uint16_t timeout) {
    if (!kbc_wait(kbc, timeout))
        return false;
    *(kbc->status) &= ~0x20;
    *(kbc->keyboard_out) = data;
    return true;
}

bool kbc_mouse(struct Kbc *const kbc, uint8_t data, uint16_t timeout) {
    if (!kbc_wait(kbc, timeout))
        return false;
    *(kbc->status) |= 0x20;
    *(kbc->mouse_out) = data;
    return true;
}
