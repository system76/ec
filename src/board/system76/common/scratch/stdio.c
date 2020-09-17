// SPDX-License-Identifier: GPL-3.0-only

#include <stdio.h>

#include <board/smfi.h>

int putchar(int c) {
    unsigned char byte = (unsigned char)c;
    smfi_debug(byte);
    return (int)byte;
}
