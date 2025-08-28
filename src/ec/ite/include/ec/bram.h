// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2020 System76, Inc.

#ifndef _EC_BRAM_H
#define _EC_BRAM_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x2200) BRAM[192];

#endif // _EC_BRAM_H
