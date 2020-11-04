// SPDX-License-Identifier: GPL-3.0-only

#include <common/macro.h>

// Prevent failures to compile on AVR
#ifndef __SDCC
    #define __code
#endif

// Magic strings read by uecflash
static const char __code UECFLASH_PROJECT[] = "PRJ:" xstr(__BOARD__) "$";
static const char __code UECFLASH_VERSION[] = "VER: 1.0.0$";
static const char __code UECFLASH_DATE[] = "DATE: " xstr(__FIRMWARE_DATE__) "$";
