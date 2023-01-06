// SPDX-License-Identifier: GPL-3.0-only

#include <common/macro.h>

// Prevent failures to compile on AVR
#ifndef __SDCC
#define __code
#endif

// clang-format off
static const char __code BOARD[] =
    "76EC_BOARD="
    xstr(__BOARD__);

static const char __code VERSION[] =
    "76EC_VERSION="
    xstr(__FIRMWARE_VERSION__);
// clang-format on

const char *board() {
    return &BOARD[11];
}

const char *version() {
    return &VERSION[13];
}
