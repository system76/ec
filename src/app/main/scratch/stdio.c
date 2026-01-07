// SPDX-License-Identifier: GPL-3.0-only

#include <stdio.h>

#include <app/smfi.h>

int putchar(int c) {
    uint8_t byte = (uint8_t)c;
    smfi_debug(byte);
    return (int)byte;
}
