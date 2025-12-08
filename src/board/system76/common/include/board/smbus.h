// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2019 System76, Inc.

#ifndef _BOARD_SMBUS_H
#define _BOARD_SMBUS_H

#include <ec/smbus.h>

void smbus_init(void);
int16_t smbus_read(uint8_t address, uint8_t command, uint16_t *const data);
int16_t smbus_write(uint8_t address, uint8_t command, uint16_t data);

#endif // _BOARD_SMBUS_H
