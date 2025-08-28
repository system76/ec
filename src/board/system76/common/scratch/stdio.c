// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2019 System76, Inc.

#include <stdio.h>

#include <board/smfi.h>

int putchar(int c) {
    uint8_t byte = (uint8_t)c;
    smfi_debug(byte);
    return (int)byte;
}
